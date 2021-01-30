#include "ramen/server.hpp"

using _server = ramen::server::Server;
using _dataqueue = ramen::dataqueue::DataQueue;
using _logholder = ramen::logholder::LogHolder;
using namespace ramen::logger;

// struct MessageRequestVote {
//   string_t type = "RequestVote";
//   uint32_t term;
//   uint32_t lastLogTerm;
//   uint32_t lastLogIndex;
// };

_server::Server() : _state(0), _term(0) {
  // Set logging level to DEBUG
  this->_logger.setLogLevel(DEBUG);

  // TODO: Change to switchState()
};

void _server::init(string_t mesh_name,
                   string_t mesh_password,
                   uint16_t mesh_port) {
  // Initialize painlessMesh
  this->_mesh.init(mesh_name, mesh_password, &_scheduler, mesh_port);
  this->_logger(DEBUG, "Just initialized painlessMesh\n");
  this->_id = _mesh.getNodeId();
};

void _server::update() {
  _mesh.update();
  requestVote();
};

void _server::switchState(uint8_t state) {};

uint8_t _server::getState() {
  return _server::_state;
};

void _server::setElectionAlarm() {
  _task_election_ptr =
      new Task(TASK_ELECTION_INTERVAL * TASK_MILLISECOND, TASK_FOREVER, [&] {
        // Check if there are nodes in the network
        bool isAloneNode = (_mesh.getNodeList(false).size() == 0);
        _logger(INFO, "I have %u mates\n", _mesh.getNodeList(false).size());

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

  this->_scheduler.addTask(*_task_election_ptr);
  _task_election_ptr->enable();
  this->_logger(DEBUG, "Set the election timer\n");
};

void _server::startNewElection() {
  auto nodeList = _mesh.getNodeList(false);

  this->_logger(INFO, "Started election: %u\n", _mesh.getNodeTime());
  this->_term += 1;
  this->_voted_for = this->_id;

  // Change state to candidate
  this->_state = 1;

  // Reinitialize list of votes granted
  this->_votes_received_ptr = new std::unordered_map<uint32_t, bool>;
  for(auto it = nodeList.begin(); it != nodeList.end(); ++it) {
    this->_votes_received_ptr->insert(std::make_pair(*it, false));
  }

  // Empty out the log holder indices of all other nodes
  this->_log.resetMatchIndexMap(&nodeList);
  this->_log.resetNextIndexMap(&nodeList);
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
  // struct MessageRequestVote message;
  // message.term = this->term;
  // message.lastLogTerm = this->log.getLastLogTerm();
  // message.lastLogIndex = this->log.getLogSize();

  DynamicJsonDocument payload(1024);
  string_t serialized_payload;

  payload["type"] = "RequestVote";
  payload["term"] = this->_term;
  payload["lastLogTerm"] = this->_log.getLastLogTerm();
  payload["lastLogIndex"] = this->_log.getLogSize();

  serializeJson(payload, serialized_payload);
  // Log(DEBUG, serialized_payload);
  Serial.begin(115200);
  Serial.println(serialized_payload);
};

void _server::handleVoteRequest(uint32_t sender, string_t data) {};
void _server::handleVoteResponse(uint32_t sender, string_t data) {};

void _server::requestAppendEntries(uint32_t receiver, string_t data) {};
void _server::handleAppendEntriesRequest(uint32_t sender, string_t data) {};
void _server::handleAppendEntriesResponse() {};

void _server::moveDataFromQueueToLog(DataQueue queue, LogHolder log) {};
void _server::sendLocalQueueDataToLeaderQueue(string_t data) {};
void _server::handleNewData(string_t data) {};