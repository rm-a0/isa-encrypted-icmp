/**
 * @file client.cpp
 * @author Michal Repcik (xrepcim00)
 */
#include "client.hpp"
#include "file_handler.hpp"
#include "encoder.hpp"
#include "chunker.hpp"
#include "protocol.hpp"
#include <iostream>

Client::Client(const std::string filePath, 
               const std::string targetAddress,
               const std::string xlogin,
               size_t maxChunkSize)
    : filePath(std::move(filePath)),
      targetAddress(std::move(targetAddress)),
      xlogin(std::move(xlogin)),
      maxChunkSize(maxChunkSize) 
      { id = generateId(); }

uint64_t Client::generateId(void) {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dist;
        return dist(gen);
}

bool Client::packageFile(Client::PacketVector& packets) {
    std::vector<uint8_t> data;

    if (!file_handler::readFile(filePath, data)) {
        return false;
    }

    std::vector<uint8_t> cipherData;
    std::vector<uint8_t> iv = encoder::generateIV();
    std::vector<uint8_t> key = encoder::deriveKey(xlogin);

    if (iv.size() != 16 || key.empty()) {
        return false;
    }

    if (!encoder::encrypt(data, key, iv, cipherData)) {
        return false;
    }

    auto chunks = chunker::chunkData(cipherData, maxChunkSize);

    protocol::Metadata meta;
    meta.fileName = file_handler::getNameFromPath(filePath);
    meta.fileSize = static_cast<uint32_t>(cipherData.size());
    meta.totalChunks = static_cast<uint32_t>(chunks.size());
    meta.iv = iv;

    auto metaPacket = protocol::buildMetadataPacket(meta, nextSeqNum++, id);
    packets.push_back(std::move(metaPacket));

    for (size_t i = 0; i < chunks.size(); ++i) {
        protocol::Data data;
        data.chunkNum = static_cast<uint32_t>(i);
        data.payload = std::move(chunks[i]);

        auto packet = protocol::buildDataPacket(data, nextSeqNum++, id);
        packets.push_back(std::move(packet));
    }

    return true;
}

bool Client::transmitPackets(PacketVector& packets, ICMPConnection& connection) {
    for (auto& packet : packets) {
        auto serialized = protocol::serializePacket(*packet);
         
        if (!connection.sendPacket(serialized.data(), serialized.size())) {
            return false;
        }
    }

    return true;
}

bool Client::run(void) {
    try {
        Client::PacketVector packets;
        if (!packageFile(packets)) {
            std::cerr << "[CLIENT] Failed to package file into packets" << std::endl;
            return false;
        }

        ICMPConnection icmpConnection(targetAddress);
        if (!icmpConnection.connect()) {
            std::cerr << "[CLIENT] Failed to establish connection to the server" << std::endl;
            return false;
        }

        if (!transmitPackets(packets, icmpConnection)) {
            std::cerr << "[CLIENT] Failed to transmit packets" << std::endl;
            return false;
        }

        return true;
    } catch (const std::invalid_argument& e) {
        std::cerr << "[CLIENT] Invalid input: " << e.what() << std::endl;
        return false;
    } catch (const std::runtime_error& e) {
        std::cerr << "[CLIENT] Runtime error: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[CLIENT] Unknown error occurred" << std::endl;
        return false;
    }
    return true;
}

