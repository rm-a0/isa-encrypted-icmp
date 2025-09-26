/**
 * @file protocol.hpp
 * @author Michal Repcik (xrepcim00)
 */
#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstdint>
#include <vector>
#include <cstddef>
#include <memory>
#include <string>
#include <variant>

/**
 * @namespace protocol
 * @brief Namespace for custom protocol for packet building and parsing
 * @warning This is a high level, not serialized packet, do not use with memcpy
 */
namespace protocol {
    /**
     * @enum Packet Type
     * @brief Type of the custom packet
     */
    enum PacketType : uint8_t {
        METADATA = 0,
        DATA = 1
    };

    /**
     * @struct Metadata
     * @brief Struct containing metadata of the packet
     */
    struct Metadata {
        uint8_t fileNameLen;            ///< Length of the filename
        std::string fileName;           ///< Name of the file
        uint32_t fileSize;              ///< Total size of the file
        uint32_t totalChunks;           ///< Expected number of chunks
        std::vector<uint8_t> iv;        ///< IV for decryption (fixed 16B)

        /**
         * @brief Serializes abstract Metadata into a vector of bytes
         * @return Byte vector
         */
        std::vector<uint8_t> serialize() const;

        /**
         * @brief Deserializes data into an abstract Metadata
         * @param data Data to be deserialized
         * @param len Length of data
         */
        static Metadata deserialize(const uint8_t* data, size_t len);
    };

    struct Data {
        uint32_t chunkNum;              ///< Number of chunk/Sequence number
        std::vector<uint8_t> payload;   ///< Data/chunk of data

        /**
         * @brief Serializes abstract data into a vector of bytes
         * @return Byte vector
         */
        std::vector<uint8_t> serialize() const;

        /**
         * @brief Deserializes data into an abstract Data
         * @param data Data to be deserialized
         * @param len Length of data
         */
        static Data deserialize(const uint8_t* data, size_t len);
    };

    /**
     * @struct Packet
     * @brief Struct containing custom packet data (6B + metadata/data)
     */
    struct Packet {
        uint32_t magicNum = 0xDEADBEEF;         ///< https://en.wikipedia.org/wiki/Magic_number_%28programming%29#Magic_debug_values
        uint8_t version = 1;                    ///< Protocol version
        PacketType packetType;                  ///< Type of the packet
        uint32_t seqNum;                         ///< Sequence number of the packet
        std::variant<Metadata, Data> payload;   ///< Custom packet payload (variant instad of unions)
    };

    using PacketPtr = std::unique_ptr<Packet>;

    /**
     * @brief Builds custom Packet
     * @param data Metadata for building packet
     * @param seqNum Sequence Number of the packet
     * @return Custom Packet containing metadata
     */
    PacketPtr buildMetadataPacket(const Metadata& data, uint32_t seqNum);

    /**
     * @brief Builds custom Packet
     * @param data Data for building packet
     * @param seqNum Sequence Number of the packet
     * @return Custom Packet containing data
     */
    PacketPtr buildDataPacket(const Data& data, uint32_t seqNum);

    /**
     * @brief Serializes any packet using their specific serialization method
     * @param packet Packet to be serialized
     * @return Serialized packet into data
     */
    std::vector<uint8_t> serializePacket(const Packet& packet);

    /**
     * @brief Parses any packet using their specific deserialization method
     * @param data Data to be deserializad
     * @param len Lenght of data
     * @return Parsed and deserialized packet
     */
    PacketPtr parsePacket(const uint8_t* data, size_t len);
}

#endif // PROTOCOL_HPP