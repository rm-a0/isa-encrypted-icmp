#include "arg_parser.hpp"
#include "file_handler.hpp"
#include <iostream>

#define MAX_CHUNK_SIZE 15000

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
    
    if (argParser.isServer()) {
        printf("Initialize server\n");
    }
    else {
        printf("Initialize client\n");
    }

    return 0;
}
