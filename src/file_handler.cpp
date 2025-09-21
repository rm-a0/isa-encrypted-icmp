#include "file_handler.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

FileHandler::FileHandler(size_t maxChunkSize) : maxChunkSize(maxChunkSize) {
    try {
        serverWriteDir = std::filesystem::current_path().string();
    } 
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to get current directory: " << e.what() << std::endl;
        serverWriteDir = ".";
    }
}

bool FileHandler::readFile(const std::string& path, std::vector<uint8_t>& data) {
    data.clear();

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file: " << path << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    if (size < 0) {
        std::cerr << "Error: Cannot determine file size: " << path << std::endl;
        file.close();
        return false;
    }

    try {
        data.resize(static_cast<size_t>(size));
    } 
    catch (const std::bad_alloc&) {
        std::cerr << "Error: Memory allocation failed for file: " << path << std::endl;
        file.close();
        return false;
    }

    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
        std::cerr << "Error: Failed to read file: " << path << std::endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool FileHandler::writeFile(const std::string& name, const std::vector<uint8_t>& data) {
    return true;
}