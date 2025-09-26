/**
 * @file server.hpp
 * @author Michal Repcik (xrepcim00)
 */
#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <condition_variable>
#include "protocol.hpp"
#include "chunker.hpp"
#include <mutex>
#include <thread>

/**
 * @class Servers
 * @brief Captures, decrypts and saves packets sent from the client
 */
class Server {
public:
    using PacketPtr = protocol::PacketPtr;
    using PacketVector = std::vector<PacketPtr>;
    /**
     * @brief Constructor for client
     * @param xlogin Login for key derivation
     */
    Server(const std::string xlogin = "xrepcim00");

    ~Server();

    /**
     * @brief Encapsulates all private sub-processes
     * @return True if no issues, False if there was an error
     * @return True if no issues, False if there was an error
     */
    bool run(void);
private:
    const std::string xlogin;               ///< Login for key derivation
    std::queue<PacketPtr> packetQueue;      ///< Shared packet queue
    std::mutex queueMutex;                  ///< Mutex used for appending/popping packets
    std::condition_variable queueCV;        ///< Condition for waiting for the thread
    std::thread consumerThread;             ///< Thread for consuming/processing packets
    std::map<uint32_t, PacketPtr> packets;  ///< Packets ordered by their sequence number
    uint32_t totalChunks;
    bool running = false;                   ///< Flag for server state

    struct PacketLoopContext {
        int headerLen;
        Server* server;
    };

    /**
     * @brief Captures packet sent from a client
     * @return True if no issues, False if there was an error
     */
    bool startPacketCapture(void);

    static void packetCaptureLoop(u_char* user, const struct pcap_pkthdr* header, const u_char* packet);

    void packetConsumerLoop(void);

    void processPackets(const protocol::Metadata& metadata,
                        chunker::ByteVector2D&& chunks);
};

#endif // SERVER_HPP
