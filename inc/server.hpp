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
#include <mutex>
#include <thread>
#include "protocol.hpp"
#include "chunker.hpp"

/**
 * @class Server
 * @brief Captures, decrypts, and saves packets sent from clients.
 */
class Server {
public:
    using PacketPtr = protocol::PacketPtr;
    using PacketVector = std::vector<PacketPtr>;

    /**
     * @brief Constructor for Server.
     * @param xlogin Login string used for key derivation (default: "xrepcim00").
     */
    explicit Server(const std::string xlogin = "xrepcim00");

    /**
     * @brief Destructor. Ensures proper cleanup of resources and threads.
     */
    ~Server();

    /**
     * @brief Starts the server workflow (capture, consume, process).
     * @return True if no issues occurred, false otherwise.
     */
    bool run(void);

private:
    const std::string xlogin;               ///< Login for key derivation
    std::queue<PacketPtr> packetQueue;      ///< Shared packet queue
    std::mutex queueMutex;                  ///< Mutex protecting packetQueue
    std::condition_variable queueCV;        ///< Condition variable for packetQueue
    std::thread consumerThread;             ///< Thread for consuming/processing packets
    bool running = false;                   ///< Server running state flag

    ///< Packet map ordered by client ID and sequence number
    std::map<uint64_t, std::map<uint32_t, PacketPtr>> clientPackets;

    ///< Map tracking total chunk count per client
    std::map<uint64_t, uint32_t> clientTotalChunks;

    struct PacketLoopContext {
        int headerLen;   ///< Length of packet header in capture
        Server* server;  ///< Pointer back to owning server
    };

    /**
     * @brief Initializes packet capture loop.
     * @return True if capture started successfully, false otherwise.
     */
    bool startPacketCapture(void);

    /**
     * @brief Callback invoked by libpcap when a packet is captured.
     * @param user User data pointer (contains PacketLoopContext).
     * @param header libpcap header for captured packet.
     * @param packet Raw packet bytes.
     */
    static void packetCaptureLoop(u_char* user, 
                                  const struct pcap_pkthdr* header, 
                                  const u_char* packet);

    /**
     * @brief Consumes packets from the queue and dispatches them for processing.
     */
    void packetConsumerLoop(void);

    /**
     * @brief Processes a fully reassembled set of client packets.
     * @param metadata Metadata describing the file/data.
     * @param chunks Vector of raw payload chunks.
     */
    void processPackets(const protocol::Metadata& metadata, 
                        chunker::ByteVector2D&& chunks);

    /**
     * @brief Updates metadata state for a given client if its first (metadata) packet is available.
     * @param clientId ID of the client whose metadata is being updated.
     */
    void updateClientMetadata(uint64_t clientId);

    /**
     * @brief Attempts to reassemble all packets for a client once enough data is received.
     * @param clientId ID of the client being checked for completion.
     */
    void tryReassemble(uint64_t clientId);
};

#endif // SERVER_HPP