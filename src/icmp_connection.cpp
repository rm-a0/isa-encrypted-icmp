/**
 * @file icmp_connection.cpp
 * @author Michal Repcik (xrepcim00)
 */
#include "icmp_connection.hpp"
#include "net_utils.hpp"
#include "protocol.hpp"
#include <iostream>
#include <unistd.h>

ICMPConnection::ICMPConnection(const std::string& targetAddress)
    : targetAddress(targetAddress), sockfd(-1) {}

ICMPConnection::~ICMPConnection() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}

bool ICMPConnection::connect(void) {
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;

    int result = net_utils::resolveAddress(targetAddress, addr4, addr6) == net_utils::ERR;
    if (result == net_utils::ERR) {
        std::cerr << "[ICMP_CONNECTION] Could not resolve target address" << std::endl;
        return false;
    }

    int protocol, domain;
    if (result == net_utils::IPv4) {
        protocol = static_cast<int>(IPPROTO_ICMP);
        domain = static_cast<int>(AF_INET);
    }
    else {
        protocol = static_cast<int>(IPPROTO_ICMPV6);
        domain = static_cast<int>(AF_INET6);
    }

    sockfd = socket(domain, SOCK_RAW, protocol);
    if (sockfd < 0) {
        std::cerr << "[ICMP_CONNECTION] Could not initialize raw socket" << std::endl;
        return false;
    }

    return true;
}

bool ICMPConnection::sendPacket(void) {
    return true;
}