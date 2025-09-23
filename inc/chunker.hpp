/**
 * @file chunker.hpp
 * @author Michal Repcik (xrepcim00)
 */
#ifndef CHUNKER_HPP
#define CHUNKER_HPP

#include <vector>
#include <cstdint>
#include <cstddef>

/**
 * @namespace chunker
 * @brief Namespace for disassembling and reassembling data
 */
namespace chunker {
    using ByteVector2D = std::vector<std::vector<uint8_t>>;
    /**
     * @brief Disassembles data to chunks of specified size
     * @param data Data to be disassembled
     * @param maxChunkSize Max size of one chunk
     * @return 2D vector containing chunked data
     */
    ByteVector2D chunkData(std::vector<uint8_t>& data, size_t maxChunkSize);

    /**
     * @brief Reassembles chunked data
     * @param chunkedData Data to be reassembled
     * @return Vector containing reassembled data
     */
    std::vector<uint8_t> reassembleData(ByteVector2D& chunkedData);
}

#endif // CHUNKER_HPP