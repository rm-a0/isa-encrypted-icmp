/**
 * @file icmp_connection.cpp
 * @author Michal Repcik (xrepcim00)
 */
#include "icmp_connection.hpp"
#include "net_utils.hpp"
#include "protocol.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <netinet/icmp6.h>

constexpr size_t MAX_PAYLOAD_SIZE = 1472;
constexpr size_t MAX_PACKET_SIZE = 1500;

ICMPConnection::ICMPConnection(const std::string& targetAddress)
    : targetAddress(targetAddress), sockfd(-1), isIPv4(false) {
    memset(&addr4, 0, sizeof(addr4));
    memset(&addr6, 0, sizeof(addr6));
}

ICMPConnection::~ICMPConnection() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}

bool ICMPConnection::connect(void) {
    int result = net_utils::resolveAddress(targetAddress, addr4, addr6) == net_utils::ERR;
    if (result == net_utils::ERR) {
        std::cerr << "[ICMP_CONNECTION] Could not resolve target address" << std::endl;
        return false;
    }

    int protocol, domain;
    if (result == net_utils::IPv4) {
        isIPv4 = true;
        protocol = static_cast<int>(IPPROTO_ICMP);
        domain = static_cast<int>(AF_INET);
    }
    else {
        isIPv4 = false;
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

bool ICMPConnection::sendPacket(const uint8_t* payload, size_t payloadSize) {
    if (payloadSize > MAX_PAYLOAD_SIZE) {
        std::cerr << "[ICMP_CONNECTION] Payload size is higher than allowed" << std::endl;
        return false;
    }
    char buffer[MAX_PACKET_SIZE];
    memset(buffer, 0, sizeof(buffer));

    size_t packetSize;
    static uint16_t sequence = 0;

    if (isIPv4) {
        struct icmphdr* icmp = reinterpret_cast<struct icmphdr*>(buffer);
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        icmp->un.echo.id = getpid() & 0xFFFF;
        icmp->un.echo.sequence = htons(++sequence);
        icmp->checksum = 0;

        memcpy(buffer + sizeof(*icmp), payload, payloadSize);
        packetSize = sizeof(*icmp) + payloadSize;

        icmp->checksum = net_utils::computeIPv4Checksum(reinterpret_cast<uint8_t*>(buffer), packetSize);

        ssize_t sent = sendto(sockfd, buffer, packetSize, 0, reinterpret_cast<struct sockaddr*>(&addr4), sizeof(addr4));
        if (sent < 0) {
            std::cerr << "[ICMP_CONNECTION] Failed to send ICMPv4 packet: " << strerror(errno) << std::endl;
            return false;
        }
    } else {
        struct icmp6_hdr* icmp6 = reinterpret_cast<struct icmp6_hdr*>(buffer);
        icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
        icmp6->icmp6_code = 0;
        icmp6->icmp6_id = getpid() & 0xFFFF;
        icmp6->icmp6_seq = htons(++sequence);
        icmp6->icmp6_cksum = 0;

        memcpy(buffer + sizeof(*icmp6), payload, payloadSize);
        packetSize = sizeof(*icmp6) + payloadSize;

        ssize_t sent = sendto(sockfd, buffer, packetSize, 0, reinterpret_cast<struct sockaddr*>(&addr6), sizeof(addr6));
        if (sent < 0) {
            std::cerr << "[ICMP_CONNECTION] Failed to send ICMPv6 packet: " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}