#include "protocol.hpp"
#include <cstring>
#include <memory>

protocol::PacketPtr protocol::buildPacket(std::vector<uint8_t>&& chunk,
                                          uint32_t seqNum,
                                          uint32_t totalChunks) {
    return std::make_unique<protocol::Packet>(std::move(chunk), 
                                              seqNum, 
                                              totalChunks);
}

protocol::PacketPtr protocol::parsePacket(const uint8_t* icmpPayload, size_t len) {
    uint32_t seqNum;
    uint32_t totalChunks;

    std::memcpy(&seqNum, icmpPayload, sizeof(uint32_t));
    std::memcpy(&totalChunks, icmpPayload + sizeof(uint32_t), sizeof(uint32_t));

    std::vector<uint8_t> chunk(icmpPayload + 2 * sizeof(uint32_t), icmpPayload + len);

    return std::make_unique<protocol::Packet>(std::move(chunk), seqNum, totalChunks);
}