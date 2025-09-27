#include "protocol.hpp"
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>

namespace protocol {

std::vector<uint8_t> Metadata::serialize() const {
    std::vector<uint8_t> out;

    uint8_t nameLen = static_cast<uint8_t>(fileName.size());
    out.push_back(nameLen);
    out.insert(out.end(), fileName.begin(), fileName.end());

    uint32_t fs = htonl(fileSize);
    out.insert(out.end(), reinterpret_cast<uint8_t*>(&fs), reinterpret_cast<uint8_t*>(&fs) + sizeof(fs));

    uint32_t tc = htonl(totalChunks);
    out.insert(out.end(), reinterpret_cast<uint8_t*>(&tc), reinterpret_cast<uint8_t*>(&tc) + sizeof(tc));

    out.insert(out.end(), iv.begin(), iv.end());

    return out;
}

Metadata Metadata::deserialize(const uint8_t* data, size_t len) {
    Metadata meta;
    size_t offset = 0;

    if (len < 1 + sizeof(uint32_t)*2) {
        throw std::runtime_error("Invalid metadata length");
    }

    uint8_t nameLen = data[offset++];
    meta.fileName = std::string(reinterpret_cast<const char*>(data + offset), nameLen);
    offset += nameLen;

    std::memcpy(&meta.fileSize, data + offset, sizeof(meta.fileSize));
    meta.fileSize = ntohl(meta.fileSize);
    offset += sizeof(meta.fileSize);

    std::memcpy(&meta.totalChunks, data + offset, sizeof(meta.totalChunks));
    meta.totalChunks = ntohl(meta.totalChunks);
    offset += sizeof(meta.totalChunks);

    meta.iv.assign(data + offset, data + len);

    return meta;
}

std::vector<uint8_t> Data::serialize() const {
    std::vector<uint8_t> out;

    uint32_t cn = htonl(chunkNum);
    out.insert(out.end(), reinterpret_cast<uint8_t*>(&cn), reinterpret_cast<uint8_t*>(&cn) + sizeof(cn));
    out.insert(out.end(), payload.begin(), payload.end());

    return out;
}

Data Data::deserialize(const uint8_t* data, size_t len) {
    Data d;

    if (len < sizeof(uint32_t)) {
        throw std::runtime_error("Invalid data packet length");
    }

    std::memcpy(&d.chunkNum, data, sizeof(d.chunkNum));
    d.chunkNum = ntohl(d.chunkNum);

    d.payload.assign(data + sizeof(d.chunkNum), data + len);
    return d;
}

PacketPtr buildMetadataPacket(const Metadata& meta, uint32_t seqNum, uint64_t clientId) {
    auto pkt = std::make_unique<Packet>();
    pkt->packetType = METADATA;
    pkt->seqNum = seqNum;
    pkt->id = clientId;
    pkt->payload = meta;
    return pkt;
}

PacketPtr buildDataPacket(const Data& d, uint32_t seqNum, uint64_t clientId) {
    auto pkt = std::make_unique<Packet>();
    pkt->packetType = DATA;
    pkt->seqNum = seqNum;
    pkt->id = clientId;
    pkt->payload = d;
    return pkt;
}

std::vector<uint8_t> serializePacket(const Packet& pkt) {
    std::vector<uint8_t> out;

    uint32_t mn = htonl(pkt.magicNum);
    out.insert(out.end(), reinterpret_cast<uint8_t*>(&mn), 
               reinterpret_cast<uint8_t*>(&mn) + sizeof(mn));

    out.push_back(pkt.version);
    out.push_back(static_cast<uint8_t>(pkt.packetType));

    uint32_t sn = htonl(pkt.seqNum);
    out.insert(out.end(), reinterpret_cast<uint8_t*>(&sn), 
               reinterpret_cast<uint8_t*>(&sn) + sizeof(sn));

    uint64_t netId = htobe64(pkt.id);
    out.insert(out.end(), reinterpret_cast<uint8_t*>(&netId),
               reinterpret_cast<uint8_t*>(&netId) + sizeof(netId));

    std::vector<uint8_t> payload;
    if (pkt.packetType == METADATA) {
        const Metadata& meta = std::get<Metadata>(pkt.payload);
        payload = meta.serialize();
    } 
    else if (pkt.packetType == DATA) {
        const Data& data = std::get<Data>(pkt.payload);
        payload = data.serialize();
    } 
    else {
        throw std::runtime_error("Unknown packet type");
    }

    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

PacketPtr parsePacket(const uint8_t* data, size_t len) {
    if (len < sizeof(uint32_t) + 2 + sizeof(uint32_t) + sizeof(uint64_t)) 
        return nullptr;

    auto pkt = std::make_unique<Packet>();
    size_t offset = 0;

    std::memcpy(&pkt->magicNum, data + offset, sizeof(pkt->magicNum));
    pkt->magicNum = ntohl(pkt->magicNum);
    offset += sizeof(pkt->magicNum);

    pkt->version = data[offset++];
    pkt->packetType = static_cast<PacketType>(data[offset++]);

    uint32_t seq;
    std::memcpy(&seq, data + offset, sizeof(seq));
    pkt->seqNum = ntohl(seq);
    offset += sizeof(seq);

    uint64_t rawId;
    std::memcpy(&rawId, data + offset, sizeof(rawId));
    pkt->id = be64toh(rawId);
    offset += sizeof(rawId);

    size_t payloadLen = len - offset;

    if (pkt->packetType == METADATA) {
        pkt->payload = Metadata::deserialize(data + offset, payloadLen);
    } 
    else if (pkt->packetType == DATA) {
        pkt->payload = Data::deserialize(data + offset, payloadLen);
    } 
    else {
        throw std::runtime_error("Unknown packet type during parse");
    }

    return pkt;
}

}