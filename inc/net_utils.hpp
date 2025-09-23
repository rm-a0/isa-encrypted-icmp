/**
 * @file net_utils.hpp
 * @author Michal Repcik (xrepcim00)
 */
#ifndef NET_UTILS_HPP
#define NET_UTILS_HPP

#include <string>
#include <stdexcept>
#include <cstdint>
#include <netinet/in.h>

namespace net_utils {
    constexpr int IPv4 = 0;
    constexpr int IPv6 = 1;
    constexpr int ERR = -1;

    /**
     * @brief Checks if the stirng is IPv4 address
     * @param str Address
     * @return True or False
     */
    bool isIPv4(const std::string& str);

    /**
     * @brief Checks if the stirng is IPv6 address
     * @param str Address
     * @return True or False
     */
    bool isIPv6(const std::string& str);

    /**
     * @brief Converts input to sockaddr (IPv4/IPv6)
     * @param input IP/hostname
     * @param addr4 IPv4 output 
     * @param addr6 IPv6 output 
     * @return 0 = IPv4, 1 = IPv6, -1 = Error
    */
    int resolveAddress(const std::string& input, 
                       struct sockaddr_in& addr4, 
                       struct sockaddr_in6& addr6);

    /**
     * @brief Calculates IPv4 checksum
     * @param data Pointer to a data
     * @param length Size of passed data
     * @return Calculated IPv4 checksum
     */
    uint16_t computeICMPChecksum(const uint8_t* data, size_t length);

    /**
     * @brief Calculates IPv6 checksum
     * @param data Pointer to a data (header)
     * @param length Size of passed data
     * @param srcAddr Source address
     * @param dstAddr Destination address
     * @return Calculated IPv6 checksum
     */
    uint16_t computeICMPv6Checksum(const uint8_t* data, size_t length, 
                                   const struct sockaddr_in6& srcAddr, 
                                   const struct sockaddr_in6& dstAddr);
}

#endif // NET_UTILS_HPP