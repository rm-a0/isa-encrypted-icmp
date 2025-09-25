/**
 * @file arg_parser.cpp
 * @author Michal Repcik (xrepcim00)
 */
#include "arg_parser.hpp"
#include <iostream>

ArgParser::ArgParser(size_t argc, char* argv[]) 
    : argc(argc), argv(argv), serverFlag(false) {}

bool ArgParser::parse(void) {
    for (size_t i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-r" && i + 1 < argc) {
            filePath = argv[++i];
        } 
        else if (arg == "-s" && i + 1 < argc) {
            targetAddress = argv[++i];
        } 
        else if (arg == "-l") {
            serverFlag = true;
        } 
        else {
            displayHelp();
            return false;
        }
    }

    if (filePath.empty() && !serverFlag) {
        std::cerr << "[ARG_PARSER] Error: Missing required argument -r <file>" << std::endl;
        return false;
    }
    if (targetAddress.empty() && !serverFlag) {
        std::cerr << "[ARG_PARSER] Error: Missing required argument -s <ip|hostname>" << std::endl;
        return false;
    }

    return true;
}

void ArgParser::displayHelp(void) {
    std::cout << "Usage: " << argv[0] << " [options]\n"
              << "\nOptions:\n"
              << "  -r <file>            Specifies the file to transfer\n"
              << "  -s <ip|hostname>     Target IP or hostname\n"
              << "  -l                   Runs the program as a server\n";
}