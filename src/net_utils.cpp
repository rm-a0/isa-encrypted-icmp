/**
 * @file net_utils.cpp
 * @author Michal Repcik (xrepcim00)
 * @note Most of the code was copied from my first IPK project (https://github.com/rm-a0/ipk-l4-scanner/blob/main/src/utils.cpp)
 *       I believe most of the implementation was inspired by these and Grok:
 *       blog: https://beej.us/guide/bgnet/html/#ip-addresses-versions-4-and-6
 *       stack overflow thread: https://stackoverflow.com/questions/5956516/getaddrinfo-and-ipv6
 */
#include "net_utils.hpp"
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <iostream>
#include <vector>

bool net_utils::isIPv4(const std::string& str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, str.c_str(), &sa.sin_addr) == 1;
}

bool net_utils::isIPv6(const std::string& str) {
    struct sockaddr_in6 sa;
    return inet_pton(AF_INET6, str.c_str(), &sa.sin6_addr) == 1;
}

int net_utils::resolveAddress(const std::string& input, 
                              struct sockaddr_in& addr4, 
                              struct sockaddr_in6& addr6) {
    std::memset(&addr4, 0, sizeof(addr4));
    std::memset(&addr6, 0, sizeof(addr6));

    if (net_utils::isIPv4(input)) {
        addr4.sin_family = AF_INET;
        addr4.sin_port = 0;
        return net_utils::IPv4;
    }

    if (net_utils::isIPv6(input)) {
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = 0;
        return net_utils::IPv6;
    }

    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_RAW;

    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_ICMP;
    if (getaddrinfo(input.c_str(), nullptr, &hints, &res) == 0) {
        for (struct addrinfo* p = res; p != nullptr; p = p->ai_next) {
            if (p->ai_family == AF_INET) {
                std::memcpy(&addr4, p->ai_addr, sizeof(struct sockaddr_in));
                freeaddrinfo(res);
                return net_utils::IPv4;
            }
        }
        freeaddrinfo(res);
    }

    hints.ai_family = AF_INET6;
    hints.ai_protocol = IPPROTO_ICMPV6;
    if (getaddrinfo(input.c_str(), nullptr, &hints, &res) == 0) {
        for (struct addrinfo* p = res; p != nullptr; p = p->ai_next) {
            if (p->ai_family == AF_INET6) {
                std::memcpy(&addr6, p->ai_addr, sizeof(struct sockaddr_in6));
                freeaddrinfo(res);
                return net_utils::IPv6;
            }
        }
        freeaddrinfo(res);
    }

    return net_utils::ERR;
}

uint16_t net_utils::computeIPv4Checksum(const uint8_t* data, size_t length) {
    if (length == 0) {
        return 0xFFFF;
    }
    uint32_t sum = 0;
    const uint16_t* ptr = reinterpret_cast<const uint16_t*>(data);
    size_t count = length;

    while (count > 1) {
        sum += *ptr++;
        count -= 2;
    }
    if (count == 1) {
        sum += static_cast<uint16_t>(*reinterpret_cast<const uint8_t*>(ptr)) << 8;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~static_cast<uint16_t>(sum);
}

uint16_t net_utils::computeIPv6Checksum(const uint8_t* data, size_t length,
                                        const struct sockaddr_in6& srcAddr,
                                        const struct sockaddr_in6& dstAddr) {
    size_t pseudoLen = 40;
    size_t totalLen = pseudoLen + length;

    std::vector<uint8_t> buf(totalLen, 0);
    size_t offset = 0;

    std::memcpy(&buf[offset], &srcAddr.sin6_addr, 16);
    offset += 16;

    std::memcpy(&buf[offset], &dstAddr.sin6_addr, 16);
    offset += 16;

    uint32_t len_net = htonl(static_cast<uint32_t>(length));
    std::memcpy(&buf[offset], &len_net, 4);
    offset += 4;

    buf[offset + 3] = IPPROTO_ICMPV6;
    offset += 4;

    std::memcpy(&buf[offset], data, length);

    return net_utils::computeIPv4Checksum(buf.data(), buf.size());
}