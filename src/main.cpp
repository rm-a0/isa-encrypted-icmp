/**
 * @file main.cpp
 * @author Michal Repcik (xrepcim00)
 */
#include "arg_parser.hpp"
#include "client.hpp"
#include "server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    ArgParser argParser(argc, argv);
    
    if (!argParser.parse()) {
        return 1;
    }

    if (argParser.isServer()) {
        Server server("xrepcim00");
        if(!server.run()) {
            return 1;
        }
    }
    else {
        Client client(argParser.getFilePath(), argParser.getTargetAddress());
        if (!client.run()) {
            return 1;
        }
    }

    return 0;
}
