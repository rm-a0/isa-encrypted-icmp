/**
 * @file server.cpp
 * @author Michal Repcik (xrepcim00)
 * @note Most of the thread releated code was inspired by ChatGPT to meet modern C++ standards and ensure thread safety
 */
#include "server.hpp"
#include "net_utils.hpp"
#include "protocol.hpp"
#include "encoder.hpp"
#include "file_handler.hpp"
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <iostream>

Server::Server(const std::string xlogin)
    : xlogin(std::move(xlogin)) {}

Server::~Server() {
    running = false;
    queueCV.notify_all();
    if (consumerThread.joinable()) {
        consumerThread.join();
    }
}

void Server::processPackets(const protocol::Metadata& metadata,
                            chunker::ByteVector2D&& chunks) {
    if (chunks.empty())
        return;

    std::vector<uint8_t> cipherData = chunker::reassembleData(chunks);
    std::vector<uint8_t> key = encoder::deriveKey(xlogin);
    std::vector<uint8_t> iv = metadata.iv;
    std::vector<uint8_t> plain;

    if (!encoder::decrypt(cipherData, key, iv, plain)) {
        std::cout << "[SERVER] Decryption of the data failed" << std::endl;
        return;
    }

    if (!file_handler::writeFile(metadata.fileName, plain)) {
        std::cout << "[SERVER] Could not save data to the file" << std::endl;
        return;
    }
}

void Server::updateClientMetadata(uint64_t clientId) {
    auto itMeta = clientPackets[clientId].find(0);
    if (itMeta != clientPackets[clientId].end() && itMeta->second) {
        if (auto metadata = std::get_if<protocol::Metadata>(&itMeta->second->payload)) {
            clientTotalChunks[clientId] = metadata->totalChunks;
        }
    }
}

void Server::tryReassemble(uint64_t clientId) {
    auto itTotal = clientTotalChunks.find(clientId);
    if (itTotal == clientTotalChunks.end())
        return;

    uint32_t totalChunks = itTotal->second;
    if (totalChunks == 0)
        return;

    if (clientPackets[clientId].size() < totalChunks + 1)
        return;

    chunker::ByteVector2D chunks;
    chunks.reserve(totalChunks);

    for (uint32_t i = 1; i <= totalChunks; ++i) {
        auto it = clientPackets[clientId].find(i);
        if (it == clientPackets[clientId].end() || !it->second) {
            std::cerr << "[SERVER] Missing packet client" << std::endl;
            return;
        }

        if (auto data = std::get_if<protocol::Data>(&it->second->payload)) {
            chunks.push_back(data->payload);
        } 
        else {
            std::cerr << "[SERVER] Unexpected packet type" << std::endl;
            return;
        }
    }

    auto itMeta = clientPackets[clientId].find(0);
    if (itMeta != clientPackets[clientId].end()) {
        if (auto metadata = std::get_if<protocol::Metadata>(&itMeta->second->payload)) {
            processPackets(*metadata, std::move(chunks));
        }
    }

    clientPackets.erase(clientId);
    clientTotalChunks.erase(clientId);
}

void Server::packetConsumerLoop(void) {
    while (running) {
        PacketPtr packet;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [this] { return !packetQueue.empty() || !running; });

            if (!running && packetQueue.empty())
                break;

            packet = std::move(packetQueue.front());
            packetQueue.pop();
            if (!packet) continue;
        }

        uint64_t clientId = packet->id;
        uint32_t seq = packet->seqNum;

        clientPackets[clientId][seq] = std::move(packet);

        updateClientMetadata(clientId);

        tryReassemble(clientId);
    }
}

void Server::packetCaptureLoop(u_char* user, const struct pcap_pkthdr* header, const u_char* packet) {
    auto* ctx = reinterpret_cast<PacketLoopContext*>(user);
    int headerLen = ctx->headerLen;
    Server* self = ctx->server;

    const u_char* ipHeader = packet + headerLen;
    uint8_t version = (*ipHeader) >> 4;


    const u_char* icmpHeader = ipHeader;
    const uint8_t* payload = nullptr;
    size_t ipHeaderLen = 0;
    size_t payloadLen = 0;
    if (version == 4) {
        const auto* iph = reinterpret_cast<const struct ip*>(ipHeader);
        ipHeaderLen = iph->ip_hl * 4;
        icmpHeader += ipHeaderLen;
        payloadLen = ntohs(iph->ip_len) - ipHeaderLen - sizeof(struct icmphdr);
        payload = reinterpret_cast<const uint8_t*>(icmpHeader + sizeof(struct icmphdr));
    } 
    else if (version == 6) {
        ipHeaderLen = sizeof(struct ip6_hdr);
        icmpHeader += ipHeaderLen;
        payloadLen = ntohs(reinterpret_cast<const struct ip6_hdr*>(ipHeader)->ip6_plen) - sizeof(struct icmp6_hdr);
        payload = reinterpret_cast<const uint8_t*>(icmpHeader + sizeof(struct icmp6_hdr));
    }

    if (!payload || payloadLen == 0) return;
    try {
        protocol::PacketPtr packetPtr = protocol::parsePacket(payload, payloadLen);

        if (packetPtr) {
            std::lock_guard<std::mutex> lock(self->queueMutex);
            self->packetQueue.push(std::move(packetPtr));
            self->queueCV.notify_one();
        }
    } catch (...) {}
}

bool Server::startPacketCapture(void) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live("any", 1500, 0, 100, errbuf);
    if (handle == nullptr) {
        std::cerr << "[SERVER] pcap_open_live failed: " << errbuf << std::endl;
        return false;
    }
    
    struct bpf_program fp;
    char filter_exp[] = "icmp[icmptype] = icmp-echo or icmp6[icmp6type] = icmp6-echo";
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "[SERVER] pcap_compiler failed: " << pcap_geterr(handle) << std::endl;
        pcap_close(handle);
        return false;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        std::cerr << "[SERVER] pcap_setfilter failed: " << pcap_geterr(handle) << std::endl;
        pcap_close(handle);
        return false;
    }
    
    int datalink = pcap_datalink(handle);
    int headerLen = net_utils::getLinkHeaderLen(datalink);
    if (headerLen < 0) {
        pcap_close(handle);
        return false;
    }

    PacketLoopContext ctx{headerLen, this};
    pcap_loop(handle, -1, packetCaptureLoop, reinterpret_cast<u_char*>(&ctx));
    
    pcap_close(handle);
    return true;
}

bool Server::run(void) {
    running = true;

    consumerThread = std::thread(&Server::packetConsumerLoop, this);

    return startPacketCapture();
}