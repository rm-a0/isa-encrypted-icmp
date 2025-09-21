#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <string>
#include <vector>

/**
 * @class FileHandler
 * @brief Handles file I/O operations and chunking
 */
class FileHandler {
public:
    /**
     * @brief Constructor for FileHandler class
     * @param maxChunkSize Maximum size of one chunk
     */
    FileHandler(size_t maxChunkSize);

    /**
     * @brief Reads from the specified file and stores its content in data
     * @param path Path to the file
     * @param data Content of the file
     * @return True if no issues, False if error occured
     */
    bool readFile(const std::string& path, std::vector<uint8_t>& data);

    /**
     * @brief Writes data to the specified file
     * @param name Name of the file
     * @param data Content to be written
     * @return True if no issues, False if error occured
     */
    bool writeFile(const std::string& name, const std::vector<uint8_t>& data);

private:
    std::string serverWriteDir;     ///< Directory for server writes
    size_t maxChunkSize;            ///< Maximum size of one chunk
};

#endif // FILE_HANDLER_HPP