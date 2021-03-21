/**
 * @file server.hpp
 * @brief server.hpp
 *
 */
#ifndef _RAMEN_SERVER_HPP_
#define _RAMEN_SERVER_HPP_

#include <unordered_map>
#include <vector>

#include "ramen/configuration.hpp"
#include "ramen/data_queue.hpp"
#include "ramen/log_holder.hpp"
#include "ramen/logger.hpp"
#include "ramen/mesh_network.hpp"
#include "ramen/message.hpp"
#include "ramen/utils.hpp"

namespace broth {
namespace server {
  using namespace broth::dataqueue;
  using namespace broth::logholder;
  using namespace broth::meshnetwork;
  using namespace broth::logger;
  using namespace broth::utils;

  /**
   * @brief Server state mapping
   *
   */
  typedef enum { FOLLOWER = 0, CANDIDATE = 1, LEADER = 2 } ServerState;

  /**
   * @brief Class that manages the consensus on the mesh network
   *
   */
  class Server {
   private:
    uint32_t _id;
    std::vector<uint32_t> _peers;
    ServerState _state;
    uint32_t _term;
    uint32_t _voted_for;
    LogHolder _log;
    uint32_t _election_alarm;
    uint32_t _previous_node_time;
    bool _received_new_append_entry_request;
    std::unordered_map<uint32_t, bool>* _votes_received_ptr = NULL;
    uint32_t _last_heart_beat;
    Logger _logger;
    MeshNetwork _mesh;
    Task* _task_election_ptr;
    uint32_t _commit_index;
    Timer _raft_timer;
    Timer _heart_beat_timer;
    Timer _request_vote_timer;

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
     *
     * @param mesh_name Name of the mesh network
     * @param mesh_password Password for the mesh network
     * @param mesh_port Port for the mesh network
     * @param logging_level Logging level for the system messages
     */
    void init(string_t mesh_name,
              string_t mesh_password,
              uint16_t mesh_port,
              uint8_t logging_level = INFO);

    /**
     * @brief Perform crucial maintenance task.
     *
     * Add this to your loop() function.
     */
    void update();

    /**
     * @brief Step down to a lower state and update term
     *
     * @param state
     * @param term
     */
    void switchState(ServerState state, uint32_t term = 0);

    /**
     * @brief Provides access to current state of server in the Raft consensus
     * (for example, follower, candidate, leader, etc)
     *
     * @return ServerState
     */
    ServerState getState();

    /**
     * @brief Set the election alarm value
     *
     */
    void setElectionAlarmValue();

    /**
     * @brief Checks for whether the timeout interval has elapsed for an
     * election
     *
     */
    void checkForElectionAlarmTimeout();

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
    void receiveData(uint32_t from, string_t& data);

    /**
     * @brief Request vote from a follower as a candidate
     *
     */
    void requestVote();

    /**
     * @brief Handle incoming vote request as a follower
     *
     * @param sender Address of the sender node
     * @param data Data received in DynamicJsonDocument format
     */
    void handleVoteRequest(uint32_t sender, DynamicJsonDocument& data);

    /**
     * @brief Handle the response of a follower to the vote request
     *
     * @param sender Address of the sender node
     * @param data Data received in DynamicJsonDocument format
     */
    void handleVoteResponse(uint32_t sender, DynamicJsonDocument& data);

    /**
     * @brief Request a follower to append an entry to its log
     *
     * @param receiver Address of the receiver node
     * @param data Data to send in JSON format
     */
    void requestAppendEntries(uint32_t receiver, string_t data);

    /**
     * @brief Parent function of requestAppendEntries
     * Sends append entry request to all nodes in the network
     *
     * @param data
     */
    void broadcastRequestAppendEntries(string_t data);

    /**
     * @brief Handle the incoming request to append an entry as a follower
     *
     * @param sender Address of the sender node
     * @param data Data received in DynamicJsonDocument format
     */
    void handleAppendEntriesRequest(uint32_t sender, DynamicJsonDocument& data);

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
} // namespace broth

#endif