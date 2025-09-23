#include "client.hpp"
#include "file_handler.hpp"
#include "encoder.hpp"
#include "chunker.hpp"
#include "protocol.hpp"
#include <iostream>

Client::Client(std::string filePath, 
               std::string targetAddress,
               std::string xlogin,
               size_t maxChunkSize)
    : filePath(std::move(filePath)), 
      targetAddress(std::move(targetAddress)), 
      xlogin(std::move(xlogin)), 
      maxChunkSize(maxChunkSize) {}

bool Client::processFile() {
    std::vector<uint8_t> data;

    std::cout << "[DEBUG] Reading file: " << filePath << std::endl;
    if (!file_handler::readFile(filePath, data)) {
        std::cerr << "[ERROR] Failed to read from file" << std::endl;
        return false;
    }

    std::cout << "[DEBUG] File size: " << data.size() << " bytes" << std::endl;

    std::vector<uint8_t> cipherData;
    std::vector<uint8_t> iv = encoder::generateIV();
    std::vector<uint8_t> key = encoder::deriveKey(xlogin);

    std::cout << "[DEBUG] Encrypting data with key derived from xlogin: " << xlogin << std::endl;
    if (!encoder::encrypt(data, key, iv, cipherData)) {
        std::cerr << "[ERROR] Encryption failed" << std::endl;
        return false;
    }

    std::cout << "[DEBUG] Encrypted data size: " << cipherData.size() << " bytes" << std::endl;

    auto chunks = chunker::chunkData(cipherData, maxChunkSize);
    std::cout << "[DEBUG] Total chunks created: " << chunks.size() << std::endl;

    protocol::Metadata meta;
    meta.fileName = filePath;
    meta.fileSize = static_cast<uint32_t>(cipherData.size());
    meta.totalChunks = static_cast<uint32_t>(chunks.size());
    meta.iv = iv;

    auto metaPacket = protocol::buildMetadataPacket(meta);
    packets.push_back(std::move(metaPacket));
    std::cout << "[DEBUG] Metadata packet prepared" << std::endl;

    for (size_t i = 0; i < chunks.size(); ++i) {
        protocol::Data d;
        d.chunkNum = static_cast<uint32_t>(i);
        d.payload = std::move(chunks[i]);

        auto packet = protocol::buildDataPacket(d);

        const auto& dataPayload = std::get<protocol::Data>(packet->payload);

        std::cout << "[DEBUG] Packet " << i + 1 << "/" << chunks.size()
                  << " payload size: " << dataPayload.payload.size() << " bytes" << std::endl;

        packets.push_back(std::move(packet));
    }

    std::cout << "[DEBUG] All packets prepared" << std::endl;
    return true;
}

bool Client::transmitPackets() {
    return true;
}
