/**
 * @file icmp_connection.hpp
 * @author Michal Repcik (xrepcim00)
 */
#ifndef ICMP_CONNECTION_HPP
#define ICMP_CONNECTION_HPP

#include <string>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <cstdint>

/**
 * @class ICMPConnection
 * @brief Class responsible for establishing and maintaining ICMP connection
 */
class ICMPConnection {
public:
    /**
     * @brief Constructor for ICMPConnection class
     * @param targetAddress IP/hostname of the server
     */
    ICMPConnection(const std::string& targetAddress);

    /**
     * @brief Desrtructor for ICMPConnection class (closes ocket)
     */
    ~ICMPConnection();

    
    /**
     * @brief Delete move and copy operators to satisfy the Rule of Five
     */ 
    ICMPConnection(const ICMPConnection&) = delete;
    ICMPConnection& operator=(const ICMPConnection&) = delete;
    ICMPConnection(ICMPConnection&&) = delete;
    ICMPConnection& operator=(ICMPConnection&&) = delete;

    /**
     * @brief Resolves target IP Address, initializes raw socekt
     * @return True if no issues, False if there was an error
     */
    bool connect(void);

    /**
     * @brief Send ICMP Packet to the target address
     * @param payload Data to be sent
     * @param payloadSize Size of the data that is supposed to be sent
     * @return True if no issues, False if there was an error
     */
    bool sendPacket(const uint8_t* payload, size_t payloadSize);
private:
    const std::string targetAddress;    ///< IP/hostname of the server
    int sockfd;                         ///< Socket
    bool isIPv4;                        ///< Protocol type
    struct sockaddr_in addr4;           ///< IPv4 address
    struct sockaddr_in6 addr6;          ///< IPv6 address
};

#endif // ICMP_CONNECTION_HPP