#include "ramen/server.hpp"

using _server = ramen::server::Server;
using _dataqueue = ramen::dataqueue::DataQueue;
using _logholder = ramen::logholder::LogHolder;
using namespace ramen::logger;

_server::Server() : state(0), term(0) {
  // Set logging level to DEBUG
  this->Log.setLogLevel(DEBUG);

  // TODO: Change to switchState()
};

void _server::init(string_t meshName, string_t meshPassword,
                   uint16_t meshPort) {
  // Initialize painlessMesh
  this->mesh.init(meshName, meshPassword, &scheduler, meshPort);
  this->Log(DEBUG, "Just initialized painlessMesh\n");
  this->id = mesh.getNodeId();
};

void _server::update() { mesh.update(); };

void _server::switchState(uint8_t state){};
uint8_t _server::getState() { return _server::state; };

void _server::setElectionAlarm() {
  task_election_ptr =
      new Task(TASK_ELECTION_INTERVAL * TASK_MILLISECOND, TASK_FOREVER, [&] {
        // Check if there are nodes in the network
        bool isAloneNode = (mesh.getNodeList(false).size() == 0);

        // Timeout if in follower mode or alone in the network
        if (this->state == 0 || isAloneNode) {
          if (this->mesh.getNodeTime() % 2) {
            // If coin toss results in 1, start a new election
            // In this way, we randomize the election duration without worrying
            // about timer overflow
            startNewElection();
          }
        }
      });

  this->scheduler.addTask(*task_election_ptr);
  task_election_ptr->enable();
  this->Log(DEBUG, "Set the election timer\n");
};

void _server::startNewElection() {
  auto nodeList = mesh.getNodeList(false);

  this->Log(INFO, "Started election: %u\n", mesh.getNodeTime());
  this->term += 1;
  this->votedFor = this->id;
  // Change state to candidate
  this->state = 1;
  // Reinitialize list of votes granted
  this->votesReceived_ptr = new std::unordered_map<uint32_t, bool>;
  for (auto it = nodeList.begin(); it != nodeList.end(); ++it) {
    this->votesReceived_ptr->insert(std::make_pair(*it, false));
  }

};

bool _server::getElectionResults() { return false; };

void _server::broadcastData(string_t data){};
void _server::sendData(uint32_t receiver, string_t data){};
string_t _server::receiveData() {
  string_t data;
  return data;
};

void _server::requestVote(uint32_t receiver, string_t data){};
void _server::handleVoteRequest(uint32_t sender, string_t data){};
void _server::handleVoteResponse(uint32_t sender, string_t data){};

void _server::requestAppendEntries(uint32_t receiver, string_t data){};
void _server::handleAppendEntriesRequest(uint32_t sender, string_t data){};
void _server::handleAppendEntriesResponse(){};

void _server::moveDataFromQueueToLog(DataQueue queue, LogHolder log){};
void _server::sendLocalQueueDataToLeaderQueue(string_t data){};
void _server::handleNewData(string_t data){};