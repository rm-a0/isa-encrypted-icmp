#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <string>
#include <vector>

/**
 * @namespace file_handler
 * @brief Namespace for file I/O operations
 */
namespace file_handler {
    /**
     * @brief Reads from the specified file and stores its content in data
     * @param path Path to the file
     * @param data Content of the file
     * @return True if no issues, False if error occurred
     */
    bool readFile(const std::string& path, std::vector<uint8_t>& data);

    /**
     * @brief Writes data to the specified file in the server directory
     * @param name Name of the file
     * @param data Content to be written
     * @return True if no issues, False if error occurred
     */
    bool writeFile(const std::string& name, const std::vector<uint8_t>& data);
}

#endif // FILE_HANDLER_HPP