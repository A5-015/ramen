#include "ramen/server.hpp"

#include "ramen/messages.hpp"

using _server = ramen::server::Server;
using _dataqueue = ramen::dataqueue::DataQueue;
using _logholder = ramen::logholder::LogHolder;
using namespace ramen::logger;

_server::Server() :
    _state(FOLLOWER), _term(0), _received_new_append_entry_request(false) {
  this->_logger.setLogLevel(INFO);
};

void _server::init(string_t mesh_name,
                   string_t mesh_password,
                   uint16_t mesh_port) {
  // Initialize painlessMesh
  this->_mesh.init(mesh_name, mesh_password, &_scheduler, mesh_port);
  this->_mesh.onReceive([&](uint32_t from, string_t data) {
    this->receiveData(from, data);
  });
  this->_id = _mesh.getNodeId();
  this->setElectionAlarmValue();

  this->_logger(DEBUG, "Just initialized painlessMesh\n");
};

void _server::update() {
  _mesh.update();

  if(this->_state == FOLLOWER) {
    this->checkForElectionAlarmTimeout();
  }

  else if(this->_state == CANDIDATE) {
    this->checkForElectionAlarmTimeout();
    this->requestVote();
  }

  else if(this->_state == LEADER) {
  }
};

void _server::switchState(ServerState state) {
  this->_state = state;
};

ramen::server::ServerState _server::getState() {
  return _server::_state;
};

void _server::setElectionAlarmValue() {
  this->_election_alarm = (1 + (std::rand() % 10)) * ELECTION_TIMEOUT_FACTOR;
  this->_logger(DEBUG, "Set the election timer to %u\n", this->_election_alarm);
};

void _server::checkForElectionAlarmTimeout() {
  uint32_t current_node_time = this->_mesh.getNodeTime();
  // Decide to start an election or not
  if(this->_received_new_append_entry_request) {
    // Reset the prev time for restarting the timer
    this->_previous_node_time = current_node_time;
    // Assume that the no new append entry requests will be received (no leaders
    // are available in the network)
    this->_received_new_append_entry_request = false;
  } else {
    if((uint32_t)(current_node_time - this->_previous_node_time) >=
       this->_election_alarm) {
      // Start an election and restart the timer
      this->startNewElection();
      this->setElectionAlarmValue();
      this->_previous_node_time = current_node_time;
    }
  }
};

void _server::startNewElection() {
  //  Get node list from painlessMesh
  auto nodeList = _mesh.getNodeList(false);

  this->_term += 1;
  this->_voted_for = this->_id;
  this->_state = CANDIDATE;

  // Reinitialize list of votes granted
  delete this->_votes_received_ptr;
  this->_votes_received_ptr = new std::unordered_map<uint32_t, bool>;

  for(auto it = nodeList.begin(); it != nodeList.end(); ++it) {
    this->_votes_received_ptr->insert(std::make_pair(*it, false));
  }

  // Empty out the log holder indices of all other nodes
  this->_log.resetMatchIndexMap(&nodeList);
  this->_log.resetNextIndexMap(&nodeList);

  this->_logger(DEBUG, "Started election at %u\n", _mesh.getNodeTime());
};

bool _server::getElectionResults() {
  return false;
};

void _server::broadcastData(string_t data) {};

void _server::sendData(uint32_t receiver, string_t data) {};

void _server::receiveData(uint32_t from, string_t& data) {
  DynamicJsonDocument payload(1024);
  deserializeJson(payload, data);
  MessageType type = payload["type"];

  switch(type) {
    case REQUEST_VOTE:
      this->handleVoteRequest(from, data);
      break;

    case SEND_VOTE:
      this->handleVoteResponse(from, data);
      break;

    default:
      break;
  }
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

void _server::handleVoteRequest(uint32_t sender, string_t& data) {
  // Generate the message
  MessageSendVote message;
  message.term = this->_term;
  // TODO: Handle vote condition
  message.granted = true;

  this->_mesh.sendSingle(sender, message.serialize());

  this->_logger(INFO, "Replied to %u with %d vote\n", sender, message.granted);
};

void _server::handleVoteResponse(uint32_t sender, string_t& data) {
  this->_logger(INFO, "Thanks dawg!\n");
};

void _server::requestAppendEntries(uint32_t receiver, string_t data) {};
void _server::handleAppendEntriesRequest(uint32_t sender, string_t data) {};
void _server::handleAppendEntriesResponse() {};

void _server::moveDataFromQueueToLog(DataQueue queue, LogHolder log) {};
void _server::sendLocalQueueDataToLeaderQueue(string_t data) {};
void _server::handleNewData(string_t data) {};