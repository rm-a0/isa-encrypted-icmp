#include "arg_parser.hpp"
#include "client.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    ArgParser argParser(argc, argv);
    
    if (!argParser.parse()) {
        return 1;
    }

    if (argParser.isServer()) {
        // Server server();
        // server.capturePackets()
    }
    else {
        Client client(argParser.getFilePath(), argParser.getTargetAddress());
        if (!client.processFile()) {
            std::cout << "Error: Client failed to process file" << std::endl;
            return 1;
        }

        if (!client.transmitPackets()) {
            std::cout << "Error: Client failed to transmit packets" << std::endl;
            return 1;
        }
    }

    return 0;
}
