#include "arg_parser.hpp"
#include "file_handler.hpp"
#include "chunker.hpp"
#include "encoder.hpp"
#include <iostream>

#define MAX_CHUNK_SIZE 14000
#define XLOGIN "xrepcim00"

int main(int argc, char* argv[]) {
    ArgParser argParser(argc, argv);
    
    if (!argParser.parse()) {
        return 1;
    }

    std::vector<uint8_t> data;
    file_handler::readFile("README.md", data);

    for (const uint8_t& i : data) {
        std::cout << i;
    }

    std::cout << std::endl << std::endl;

    std::string login = XLOGIN;
    std::vector<uint8_t> cipherData;
    std::vector<uint8_t> iv = encoder::generateIV();
    std::vector<uint8_t> key = encoder::deriveKey(login);
    encoder::encrypt(data, key, iv, cipherData);

    for (const uint8_t& i : cipherData) {
        std::cout << i;
    }

    std::cout << std::endl << std::endl;

    std::vector<std::vector<uint8_t>> chunkedData = chunker::chunkData(cipherData, MAX_CHUNK_SIZE);
    for (const auto& i : chunkedData) {
        std::cout << i.data() << std::endl;
    }

    std::vector<uint8_t> cipherData2 = chunker::reassembleData(chunkedData);

    std::cout << std::endl << std::endl;

    std::vector<uint8_t> decipherData;
    encoder::decrypt(cipherData2, key, iv, decipherData);

    for (const uint8_t& i : decipherData) {
        std::cout << i;
    }
    
    std::cout << std::endl << std::endl;

    if (argParser.isServer()) {
        //printf("Initialize server\n");
    }
    else {
        //printf("Initialize client\n");
    }

    return 0;
}
