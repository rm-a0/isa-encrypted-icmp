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
 * @brief 
 */
class ICMPConnection {
public:

    ICMPConnection(const std::string& targetAddress);

    ~ICMPConnection();

    bool connect(void);

    bool sendPacket(void);
private:
    const std::string targetAddress;
    int sockfd;
};

#endif // ICMP_CONNECTION_HPP