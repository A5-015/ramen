/**
 * @file server.cpp
 * @brief server.cpp
 *
 */
#include "ramen/server.hpp"

#include "ramen/messages.hpp"

using _server = broth::server::Server;
using _dataqueue = broth::dataqueue::DataQueue;
using _logholder = broth::logholder::LogHolder;
using namespace broth::logger;

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
  this->_logger.setLoggerId(_mesh.getNodeId());
  this->_previous_node_time = _mesh.getNodeTime();
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

void _server::switchState(ServerState state, uint32_t term) {
  switch(state) {
    case LEADER: {
      auto nodeList = _mesh.getNodeList(false);
      this->_state = LEADER;
      this->_log.resetNextIndexMap(&nodeList, this->_log.getLogSize() + 1);
      this->_election_alarm = INFINITY;
      break;
    }
    case CANDIDATE: {
      break;
    }
    case FOLLOWER: {
      // Reset election alarm if stepping down from being a leader
      if(this->_state == LEADER) {
        this->setElectionAlarmValue();
      }
      // Set new state and update term
      this->_state = state;
      this->_term = term;
      this->_voted_for = 0;
      break;
    }
    default:
      break;
  }
};

void _server::setElectionAlarmValue() {
  this->_election_alarm = (1 + (std::rand() % 10)) * ELECTION_TIMEOUT_FACTOR;
  this->_previous_node_time = _mesh.getNodeTime();
  this->_logger(
      DEBUG,
      "Set the election alarm %u duration from now, which is %u @ mesh time\n",
      this->_election_alarm,
      _mesh.getNodeTime() + this->_election_alarm);
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
      this->_logger(DEBUG,
                    "Current time = %u, Previous time = %u, Difference = %u\n",
                    current_node_time,
                    this->_previous_node_time,
                    (uint32_t)(current_node_time - this->_previous_node_time));
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
  this->_log.resetMatchIndexMap(&nodeList, 0);
  this->_log.resetNextIndexMap(&nodeList, 1);

  this->_logger(DEBUG, "Started election at %u\n", _mesh.getNodeTime());
};

bool _server::getElectionResults() {
  // Initialize to one because node votes for itself
  uint32_t granted_votes = 1;

  // Iterate through map to count granted votes
  for(auto vote : *(this->_votes_received_ptr)) {
    if(vote.second) {
      ++granted_votes;
    }
  }

  this->_logger(
      DEBUG,
      "I have %u votes and more than %u votes is enough to win the election\n",
      granted_votes,
      this->_votes_received_ptr->size() / 2);

  // Majority decides election win
  bool won_election = (granted_votes > (this->_votes_received_ptr->size() / 2));

  return won_election;
};

void _server::broadcastData(string_t data) {};

void _server::sendData(uint32_t receiver, string_t data) {};

void _server::receiveData(uint32_t from, string_t& data) {
  DynamicJsonDocument payload(1024);
  deserializeJson(payload, data);
  MessageType type = payload["type"];

  switch(type) {
    case REQUEST_VOTE:
      this->handleVoteRequest(from, payload);
      break;

    case SEND_VOTE:
      this->handleVoteResponse(from, payload);
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

void _server::handleVoteRequest(uint32_t sender, DynamicJsonDocument& data) {
  // Equalize term with sender if term is lower
  if(this->_term < data["term"]) {
    this->switchState(FOLLOWER, data["term"]);
  }

  // Default vote grant to false
  bool granted = false;

  // If term is equal to sender && (haven't voted yet || voted for sender
  // before)
  // clang-format off
  if(this->_term == data["term"] &&
     (this->_voted_for == 0 || this->_voted_for == sender)) {

    // If log term and log size are greater than or equal to receiver's term and
    // size
    if(data["lastLogTerm"] >= this->_log.getLastLogTerm() &&
       data["lastLogIndex"] >= this->_log.getLogSize()) {

      // then vote for sender and reset alarm
      granted = true;
      this->_voted_for = sender;
      this->setElectionAlarmValue();
    }
  }
  // clang-format on

  // Generate the message
  MessageSendVote message;
  message.term = this->_term;
  message.granted = granted;

  this->_mesh.sendSingle(sender, message.serialize());

  this->_logger(INFO, "Replied to %u with %u vote\n", sender, message.granted);
};

void _server::handleVoteResponse(uint32_t sender, DynamicJsonDocument& data) {
  // Equalize term with responder if term is lower
  if(this->_term < data["term"]) {
    this->switchState(FOLLOWER, data["term"]);
  }

  // Update votes received map
  bool granted = data["granted"];
  if(this->_state == CANDIDATE && this->_term == data["term"]) {
    // Store received vote in votes received map
    (*(this->_votes_received_ptr))[sender] = granted;
    this->_logger(INFO, "Saved vote from %u with %u vote\n", sender, granted);

    // Check for election results
    if(this->getElectionResults()) {
      this->_logger(DEBUG, "Won the election at %u\n", _mesh.getNodeTime());
      this->switchState(LEADER);
    } else {
      this->_logger(DEBUG,
                    "Did not win the election at %u\n",
                    _mesh.getNodeTime());
    }
  }
};

void _server::requestAppendEntries(uint32_t receiver, string_t data) {};
void _server::handleAppendEntriesRequest(uint32_t sender, string_t data) {};
void _server::handleAppendEntriesResponse() {};

void _server::moveDataFromQueueToLog(DataQueue queue, LogHolder log) {};
void _server::sendLocalQueueDataToLeaderQueue(string_t data) {};
void _server::handleNewData(string_t data) {};