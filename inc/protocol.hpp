#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstdint>
#include <vector>
#include <cstddef>
#include <memory>

/**
 * @namespace protocol
 * @brief Namespace for custom protocol for packet building and parsing
 */
namespace protocol {
    /**
     * @struct Packet
     * @brief Struct containing custom packet data
     */
    struct Packet {
        uint32_t seqNum;                ///< Sequence number of chunk
        uint32_t totalChunks;           ///< Total/expected number of chunks
        std::vector<uint8_t> payload;   ///< Data/chunk of data

        Packet(std::vector<uint8_t>&& chunk, uint32_t seq, uint32_t total)
        : seqNum(seq), totalChunks(total), payload(std::move(chunk)) {}
    };

    using PacketPtr = std::unique_ptr<Packet>;

    /**
     * @brief Builds custom Packet
     * @param chunk Chunk of data
     * @param seqNum Sequence numbr of the chunk
     * @param totalChunks Total/expected number of chunks
     * @return Custom Packet
     */
    PacketPtr buildPacket(std::vector<uint8_t>&& chunk, 
                                        uint32_t seqNum, 
                                        uint32_t totalChunks);

    /**
     * @brief Parser icmp payload and constructs custom Packet
     * @param icmpPayload Receicved icmp data
     * @param len Length of data
     * @return Custom Packet
     */
    PacketPtr parsePacket(const uint8_t* icmpPayload, size_t len);
}

#endif // PROTOCOL_HPP