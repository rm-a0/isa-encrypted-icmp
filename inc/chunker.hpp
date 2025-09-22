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
    /**
     * @brief Disassembles data to chunks of specified size
     * @param data Data to be disassembled
     * @param maxChunkSize Max size of one chunk
     * @return 2D vector containing chunked data
     */
    std::vector<std::vector<uint8_t>> chunkData(std::vector<uint8_t>& data, size_t maxChunkSize);

    /**
     * @brief Reassembles chunked data
     * @param chunkedData Data to be reassembled
     * @return Vector containing reassembled data
     */
    std::vector<uint8_t> reassembleData(std::vector<std::vector<uint8_t>>& chunkedData);
}

#endif // CHUNKER_HPP