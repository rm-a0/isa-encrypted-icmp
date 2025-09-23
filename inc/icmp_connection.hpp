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
 * @brief 
 */
class ICMPConnection {
public:

    ICMPConnection(const std::string& targetAddress);

    ~ICMPConnection();

    bool sendPacket();
private:
    uint16_t computeChecksum(const uint8_t* buffer, size_t size);

    bool convertToIPv4(const std::string& targetAddress, sockaddr_in ipv4);

    struct sockaddr_in dest_addr_;
};

#endif // ICMP_CONNECTION_HPP