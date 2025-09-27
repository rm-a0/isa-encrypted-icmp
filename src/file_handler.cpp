/**
 * @file file_handler.cpp
 * @author Michal Repcik (xrepcim00)
 */
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

bool file_handler::writeFile(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << path << std::endl;
        return false;
    }

    if (!data.empty()) {
        if (!file.write(reinterpret_cast<const char*>(data.data()), 
                        static_cast<std::streamsize>(data.size()))) {
            std::cerr << "Error: Failed to write to file: " << path << std::endl;
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

std::string file_handler::getNameFromPath(const std::string& path) {
    try {
        std::filesystem::path fsPath(path);
        return fsPath.filename().string();
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid path: " << path << " (" << e.what() << ")" << std::endl;
        return {};
    }
}