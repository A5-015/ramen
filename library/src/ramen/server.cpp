#include "ramen/server.hpp"

#include "ramen/messages.hpp"

using _server = ramen::server::Server;
using _dataqueue = ramen::dataqueue::DataQueue;
using _logholder = ramen::logholder::LogHolder;
using namespace ramen::logger;

_server::Server() : _state(FOLLOWER), _term(0) {
  // Set logging level to DEBUG
  this->_logger.setLogLevel(DEBUG);
};

void _server::init(string_t mesh_name,
                   string_t mesh_password,
                   uint16_t mesh_port) {
  // Initialize painlessMesh
  this->_mesh.init(mesh_name, mesh_password, &_scheduler, mesh_port);
  this->_id = _mesh.getNodeId();

  this->_logger(DEBUG, "Just initialized painlessMesh\n");
};

void _server::update() {
  _mesh.update();
};

void _server::switchState(ServerState state) {
  this->_state = state;
};

ramen::server::ServerState _server::getState() {
  return _server::_state;
};

void _server::setElectionAlarm() {
  // Create a new election alarm task for the scheduler
  _task_election_ptr =
      new Task(TASK_ELECTION_INTERVAL * TASK_MILLISECOND, TASK_FOREVER, [&] {
        // Check if there are nodes in the network
        bool isAloneNode = (_mesh.getNodeList(false).size() == 0);

        // Timeout if in follower mode or alone in the network
        if(this->_state == 0 || isAloneNode) {
          if(this->_mesh.getNodeTime() % 2) {
            // If coin toss results in 1, start a new election
            // In this way, we randomize the election duration without worrying
            // about timer overflow
            startNewElection();
          }
        }
      });

  // Add the alarm to scheduler and enable it
  this->_scheduler.addTask(*_task_election_ptr);
  _task_election_ptr->enable();

  this->_logger(DEBUG, "Set the election timer\n");
};

void _server::startNewElection() {
  //  Get node list from painlessMesh
  auto nodeList = _mesh.getNodeList(false);

  this->_term += 1;
  this->_voted_for = this->_id;
  this->_state = CANDIDATE;

  // Reinitialize list of votes granted
  this->_votes_received_ptr = new std::unordered_map<uint32_t, bool>;
  for(auto it = nodeList.begin(); it != nodeList.end(); ++it) {
    this->_votes_received_ptr->insert(std::make_pair(*it, false));
  }

  // Empty out the log holder indices of all other nodes
  this->_log.resetMatchIndexMap(&nodeList);
  this->_log.resetNextIndexMap(&nodeList);

  this->_logger(INFO, "Started election at %u\n", _mesh.getNodeTime());
};

bool _server::getElectionResults() {
  return false;
};

void _server::broadcastData(string_t data) {};

void _server::sendData(uint32_t receiver, string_t data) {};

string_t _server::receiveData() {
  string_t data;
  return data;
};

void _server::requestVote() {
  // Generate the message
  MessageRequestVote message;
  message.term = this->_term;
  message.last_log_term = this->_log.getLastLogTerm();
  message.last_log_index = this->_log.getLogSize();

  // Broadcast the message
  _mesh.sendBroadcast(message.serialize());

  this->_logger(DEBUG, "Requested vote from other nodes\n");
};

void _server::handleVoteRequest(uint32_t sender, string_t data) {};
void _server::handleVoteResponse(uint32_t sender, string_t data) {};

void _server::requestAppendEntries(uint32_t receiver, string_t data) {};
void _server::handleAppendEntriesRequest(uint32_t sender, string_t data) {};
void _server::handleAppendEntriesResponse() {};

void _server::moveDataFromQueueToLog(DataQueue queue, LogHolder log) {};
void _server::sendLocalQueueDataToLeaderQueue(string_t data) {};
void _server::handleNewData(string_t data) {};