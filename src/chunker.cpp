#include "chunker.hpp"

std::vector<std::vector<uint8_t>> chunker::chunkData(std::vector<uint8_t>& data, size_t maxChunkSize) {
    std::vector<std::vector<uint8_t>> chunks;
    size_t offset = 0;

    while (offset < data.size()) {
        size_t size = std::min(maxChunkSize, data.size() - offset);
        chunks.emplace_back(data.begin() + offset, data.begin() + offset + size);
        offset += size;
    }

    return chunks;
}

std::vector<uint8_t> chunker::reassembleData(std::vector<std::vector<uint8_t>>& chunkedData) {
    size_t totalSize = 0;
    for (const auto& chunk : chunkedData) {
        totalSize += chunk.size();
    }

    std::vector<uint8_t> data;
    data.reserve(totalSize);

    for (const auto& chunk : chunkedData) {
        data.insert(data.end(), chunk.begin(), chunk.end());
    }

    return data;
}