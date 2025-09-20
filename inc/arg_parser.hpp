#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <string>

class ArgParser {
public:
    ArgParser(int argc, char* argv[]);
    bool parse();
    bool isServer() const { return serverFlag; }
    std::string getFilePath() const { return filePath; }
    void displayHelp();

private:
    int argc;
    char** argv;
    std::string filePath;
    std::string targetAddress;
    bool serverFlag;
};

#endif // ARG_PARSER_HPP