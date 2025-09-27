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
    bool running = false;                   ///< Flag for server state
    ///< Packet map ordered by client ID and seqNum
    std::map<uint64_t, std::map<uint32_t, PacketPtr>> clientPackets;
    ///< Map for tracking client ID and per-client chunks
    std::map<uint64_t, uint32_t> clientTotalChunks;                     

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

    void updateClientMetadata(uint64_t clientId);

    void tryReassemble(uint64_t clientId);
};

#endif // SERVER_HPP
