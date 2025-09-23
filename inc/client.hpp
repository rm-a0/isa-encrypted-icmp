/**
 * @file client.hpp
 * @author Michal Repcik (xrepcim00)
 */
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
    Client(const std::string filePath, 
           const std::string targetAddress,
           const std::string xlogin = "xrepcim00", 
           size_t maxChunkSize = 1400);

    /**
     * @brief Encapsulates all private sub-processes
     * @return True if no issues, False if there was an error
     */
    bool run(void);

private:
    const std::string filePath;         ///< Path to the file
    const std::string targetAddress;    ///< Target IP or hostname
    const std::string xlogin;           ///< xlogin for key derivation
    size_t maxChunkSize;                ///< Maximum chunk size

    /**
     * @brief Process file - read, encrypt, chunk and packet file
     * @param packets Output vector containing all file related packets
     * @return True if no issues, False if there was an error
     */
    bool packageFile(PacketVector& packets);


    /**
     * @brief Serialize packets, consturct icmp ones, send them to the target
     * @return True if no issues, False if there was an error
     */
    bool transmitPackets(void);
};

#endif // CLIENT_HPP