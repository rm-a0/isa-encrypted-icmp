#include "client.hpp"
#include "file_handler.hpp"
#include "encoder.hpp"
#include "chunker.hpp"
#include <iostream>

Client::Client(std::string filePath, 
               std::string targetAddress,
               std::string xlogin,
               size_t maxChunkSize)
    : filePath(filePath), 
      targetAddress(targetAddress), 
      xlogin(xlogin), 
      maxChunkSize(maxChunkSize) {}

bool Client::processFile(void) {
    std::vector<uint8_t> data;

    std::cout << "[DEBUG] Reading file: " << filePath << std::endl;
    if (!file_handler::readFile(filePath, data)) {
        std::cerr << "[ERROR] Failed to read from file" << std::endl;
        return false;
    }

    std::cout << "[DEBUG] File size: " << data.size() << " bytes" << std::endl;

    // Encrypt
    std::vector<uint8_t> cipherData;
    std::vector<uint8_t> iv = encoder::generateIV();
    std::vector<uint8_t> key = encoder::deriveKey(xlogin);

    std::cout << "[DEBUG] Encrypting data with key derived from xlogin: " << xlogin << std::endl;
    if (!encoder::encrypt(data, key, iv, cipherData)) {
        std::cerr << "[ERROR] Encryption failed" << std::endl;
        return false;
    }

    std::cout << "[DEBUG] Encrypted data size: " << cipherData.size() << " bytes" << std::endl;

    // Chunking
    auto chunks = chunker::chunkData(cipherData, maxChunkSize);
    std::cout << "[DEBUG] Total chunks created: " << chunks.size() << std::endl;

    for (size_t i = 0; i < chunks.size(); ++i) {
        auto packet = protocol::buildPacket(std::move(chunks[i]), i, chunks.size());
        std::cout << "[DEBUG] Packet " << i + 1 << "/" << chunks.size() 
                  << " payload size: " << packet->payload.size() << " bytes" << std::endl;
        packets.push_back(std::move(packet));
    }

    std::cout << "[DEBUG] All packets prepared" << std::endl;
    return true;
}
