#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <protocol.hpp>

/**
 * @class Client
 * @brief Manages, encrypts and transmits specified file to a server
 */
class Client {
public:
    using PacketVector = std::vector<protocol::PacketPtr>;

    /**
     * @brief Constructor for Client class
     * @param filePath Path to the file
     * @param targetAddress Target ip or hostname
     * @param xlogin Login for key derivation
     * @param maxChunkSize Maximum chunk size
     */
    Client(std::string filePath, 
           std::string targetAddress,
           std::string xlogin = "xrepcim00", 
           size_t maxChunkSize = 13000);

    bool processFile(void);

    bool transmitPackets(void);

private:
    std::string filePath;           ///< Path to the file
    std::string targetAddress;      ///< Target IP or hostname
    std::string xlogin;             ///< xlogin for key derivation
    size_t maxChunkSize;            ///< Maximum chunk size
    PacketVector packets;           ///< Vector of packets to be sent
};

#endif // CLIENT_HPP