#include "arg_parser.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    ArgParser argParser(argc, argv);
    
    if (!argParser.parse()) {
        return 1;
    }

    return 0;
}
