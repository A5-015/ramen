#ifndef _RAMEN_SERVER_HPP_
#define _RAMEN_SERVER_HPP_

#include <unordered_map>
#include <vector>

#include "ramen/configuration.hpp"
#include "ramen/data_queue.hpp"
#include "ramen/log_holder.hpp"
#include "ramen/logger.hpp"

namespace ramen {
namespace server {
  using namespace ramen::dataqueue;
  using namespace ramen::logholder;
  using namespace ramen::logger;

  /**
   * @brief Class that manages the consensus on the mesh network
   *
   */
  class Server {
   private:
    uint32_t id;
    std::vector<uint32_t> peers;
    uint8_t state;
    uint32_t term;
    uint32_t votedFor;
    LogHolder log;
    uint16_t electionAlarm;
    std::unordered_map<uint32_t, bool> *votesReceived_ptr;
    uint32_t lastHeartBeat;
    Logger Log;
    painlessMesh mesh;
    Scheduler scheduler;
    Task *task_election_ptr;

   public:
    /**
     * @brief Construct a new Server object
     *
     */
    Server();

    /**
     * @brief Initializes the raft consensus on the mesh network
     *
     * Add this to your setup() function.
     */
    void init(string_t meshName, string_t meshPassword, uint16_t meshPort);

    /**
     * @brief Perform crucial maintenance task.
     *
     * Add this to your loop() function.
     */
    void update();

    /**
     * @brief Switch between server states in consensus
     *
     * @param state 0 for follower, 1 for candidate, and 2 for leader
     */
    void switchState(uint8_t state);

    /**
     * @brief Get the State object
     *
     * @return uint8_t
     */
    uint8_t getState();

    /**
     * @brief Set the Election Alarm object
     *
     */
    void setElectionAlarm();

    /**
     * @brief Start a new election
     *
     */
    void startNewElection();

    /**
     * @brief Get the Election Results object
     *
     * @return true
     * @return false
     */
    bool getElectionResults();

    /**
     * @brief Broadcast a data string to the whole mesh network
     *
     * @param data Data to broadcast in JSON format
     */
    void broadcastData(string_t data);

    /**
     * @brief Send a data string to a node in the mesh network
     *
     * @param receiver Address of the receiver node
     * @param data Data to send in JSON format
     */
    void sendData(uint32_t receiver, string_t data);

    /**
     * @brief Handles incoming data string from the mesh network
     *
     * @return std::string
     */
    string_t receiveData();

    /**
     * @brief Request vote from a follower as a candidate
     *
     */
    void requestVote();

    /**
     * @brief Handle incoming vote request as a follower
     *
     * @param sender Address of the sender node
     * @param data Data received in JSON format
     */
    void handleVoteRequest(uint32_t sender, string_t data);

    /**
     * @brief Handle the response of a follower to the vote request
     *
     * @param sender Address of the sender node
     * @param data Data received in JSON format
     */
    void handleVoteResponse(uint32_t sender, string_t data);

    /**
     * @brief Request a follower to append an entry to its log
     *
     * @param receiver Address of the receiver node
     * @param data Data to send in JSON format
     */
    void requestAppendEntries(uint32_t receiver, string_t data);

    /**
     * @brief Handle the incoming request to append an entry as a follower
     *
     * @param sender Address of the sender node
     * @param data Data received in JSON format
     */
    void handleAppendEntriesRequest(uint32_t sender, string_t data);

    /**
     * @brief Handle the response of a follower to append an entry
     *
     */
    void handleAppendEntriesResponse();

    /**
     * @brief For a leader, move the data available in the queue to the
     * consensus log
     *
     * @param queue
     * @param log
     */
    void moveDataFromQueueToLog(DataQueue queue, LogHolder log);

    /**
     * @brief Send the data available in the local queue to the consensus
     * leader and remove if from the queue
     *
     * @param data Data to send in JSON format
     */
    void sendLocalQueueDataToLeaderQueue(string_t data);

    /**
     * @brief Handle the data generated by a client and place it into the
     * queue
     *
     * @param data Data received in JSON format
     */
    void handleNewData(string_t data);
  };

} // namespace server
} // namespace ramen

#endif