#ifndef _RAMEN_SERVER_HPP_
#define _RAMEN_SERVER_HPP_

#include <string>
#include <unordered_map>
#include <vector>

#include "ramen/configuration.hpp"
#include "ramen/data_queue.hpp"
#include "ramen/log_holder.hpp"

namespace ramen {

/**
 * @brief Class that manages the consensus on the mesh network
 *
 */
namespace server {
class Server {
 private:
  uint32_t id;
  std::vector<uint32_t> peers;
  uint8_t state;
  uint32_t term;
  uint32_t votedFor;
  LogHolder::LogHolder log;
  uint16_t electionAlarm;
  std::unordered_map<uint32_t, bool> votesReceived;
  uint32_t lastHeartBeat;

 public:
  void init();

  void switchMode(uint8_t mode);
  uint8_t getMode();

  void setElectionAlarm();
  void startNewElection_candidate();
  bool getElectionResults();

  void broadcastData(std::string data);
  void sendData();
  std::string receiveData();

  void requestVote(uint32_t receiver, std::string data);
  void handleVoteRequest(uint32_t sender, std::string data);
  void handleVoteResponse(uint32_t sender, std::string data);

  void requestAppendEntries(uint32_t receiver, std::string data);
  void handleAppendEntriesRequest(uint32_t sender, std::string data);
  void handleAppendEntriesResponse();

  void moveDataFromQueueToLog(DataQueue::DataQueue queue,
                              LogHolder::LogHolder log);
  void sendLocalQueueDataToLeaderQueue(std::string data);
  void handleNewData(std::string data);
};

}  // namespace server
}  // namespace ramen

#endif