#include "file_handler.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

bool file_handler::readFile(const std::string& path, std::vector<uint8_t>& data) {
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

bool file_handler::writeFile(const std::string& name, const std::vector<uint8_t>& data) {
    return true;
}