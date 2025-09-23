#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <string>

/**
 * @class ArgParser
 * @brief Parses and stores command line arguments
 */
class ArgParser {
public:
    /**
     * @brief Constructor for ArgParser class
     * @param argc Argument count
     * @param argv List of arguments
     */
    ArgParser(int argc, char* argv[]);

    /**
     * @brief Parses command line arguments
     * @return True if no issues, False if there was an error
     */
    bool parse(void);

    /**
     * @brief Displays help message (expected arguments)
     * @return void
     */
    void displayHelp(void);

    /**
     * @brief Getters for better encapsulation and safety
     */
    bool isServer() const { return serverFlag; }
    std::string getFilePath() const { return filePath; }
    std::string getTargetAddress() const { return targetAddress; }

private:
    int argc;                   ///< Argument count
    char** argv;                ///< Arguments
    std::string filePath;       ///< Path to the file
    std::string targetAddress;  ///< Target IP or hostname
    bool serverFlag;            ///< Flag for server initialization
};

#endif // ARG_PARSER_HPP