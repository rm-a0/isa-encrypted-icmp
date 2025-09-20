#include "arg_parser.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    ArgParser argParser(argc, argv);
    
    if (!argParser.parse()) {
        return 1;
    }

    if (argParser.isServer()) {
        printf("Initialize server\n");
    }
    else {
        printf("Initialize client\n");
    }

    return 0;
}
