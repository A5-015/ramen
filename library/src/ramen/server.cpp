/**
 * @file server.cpp
 * @brief server.cpp
 *
 */
#include "ramen/server.hpp"

using _server = broth::server::Server;
using _dataqueue = broth::dataqueue::DataQueue;
using _logholder = broth::logholder::LogHolder;
using _meshnetwork = broth::meshnetwork::MeshNetwork;
using namespace broth::server;
using namespace broth::utils;
using namespace broth::message;
using namespace broth::logger;

_server::Server() :
    _state(FOLLOWER),
    _term(0),
    _received_new_append_entry_request(false) {

    };

void _server::init(string_t mesh_name,
                   string_t mesh_password,
                   uint16_t mesh_port,
                   uint8_t logging_level) {
  this->_logger.setLogLevel(logging_level);

  // Initialize painlessMesh
  this->_mesh.init(PAINLESSMESH,
                   mesh_name,
                   mesh_password,
                   mesh_port,
                   logging_level);

  // Set callbacks
  this->_mesh.onReceiveCallback([&](uint32_t from, string_t data) {
    this->receiveData(from, data);
  });

  // Set node ID and time
  this->_id = this->_mesh.getNodeId();
  this->_logger.setLoggerId(this->_mesh.getNodeId());
  this->_previous_node_time = this->_mesh.getNodeTime();

  // Set the election alarm
  this->setElectionAlarmValue();

  // Let the user know that initialization was successful
  this->_logger(DEBUG, "Just initialized the node!\n");

  // Set timers' periods to specified periods
  this->_raft_timer.init(RAFT_TIMER_PERIOD);
  this->_request_vote_timer.init(REQUEST_VOTE_TIMER_PERIOD);
  this->_heart_beat_timer.init(HEART_BEAT_TIMER_PERIOD);
  this->_request_append_entry_timer.init(REQUEST_APPEND_ENTRY_PERIOD);
};

void _server::update() {
  // Update the mesh network all the time
  this->_mesh.update();

  uint32_t current_time = this->_mesh.getNodeTime();

  // Slow down the Raft implementation with the help of the timer
  if(this->_raft_timer.check(current_time)) {
    //////////////////////
    // State => FOLLOWER
    //////////////////////
    if(this->_state == FOLLOWER) {
      this->checkForElectionAlarmTimeout();
    }

    //////////////////////
    // State => CANDIDATE
    //////////////////////
    else if(this->_state == CANDIDATE) {
      this->checkForElectionAlarmTimeout();

      // Slow down with timer
      if(this->_request_vote_timer.check(current_time)) {
        this->requestVote();
      }
    }

    //////////////////////
    // State => LEADER
    //////////////////////
    else if(this->_state == LEADER) {
      this->_commit_index =
          std::max(this->_commit_index, this->_log.getMajorityCommitIndex());

      // Slow down with timer
      bool heart_beat_timer = this->_heart_beat_timer.check(current_time);
      bool request_append_entry_timer =
          this->_request_append_entry_timer.check(current_time);

      // If heart_beat_timer and request_append_entry_timer timeout together,
      // send append entry request,
      // otherwise send an hear beat
      if(heart_beat_timer && request_append_entry_timer) {
        // Boolean refers to sending heartbeat
        this->broadcastRequestAppendEntries(false);
      } else if(heart_beat_timer) {
        this->broadcastRequestAppendEntries(true);
      }
    }

    this->sendLocalQueueDataToLeaderQueue();
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

ServerState _server::getState() {
  return this->_state;
};

void _server::setElectionAlarmValue(uint32_t min_val) {
  this->_election_alarm =
      (min_val + (std::rand() % 10)) * ELECTION_TIMEOUT_FACTOR;
  this->_previous_node_time = _mesh.getNodeTime();
  this->_logger(DEBUG,
                "Set the election alarm %u duration from now, which is %u @ "
                "mesh time\n",
                this->_election_alarm,
                this->_mesh.getNodeTime() + this->_election_alarm);
};

void _server::checkForElectionAlarmTimeout() {
  uint32_t current_node_time = this->_mesh.getNodeTime();
  // Decide to start an election or not
  if(this->_received_new_append_entry_request) {
    // Reset the prev time for restarting the timer
    this->_previous_node_time = current_node_time;
    // Assume that the no new append entry requests will be received (no
    // leaders are available in the network)
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

  this->_logger(DEBUG,
                "I have %u votes and more than %u votes is enough to win the "
                "election\n",
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
  MessageType type = payload[TYPE_FIELD_KEY];

  // router
  switch(type) {
    case REQUEST_VOTE:
      this->handleVoteRequest(from, payload);
      break;

    case SEND_VOTE:
      this->handleVoteResponse(from, payload);
      break;

    case REQUEST_APPEND_ENTRY:
      this->handleAppendEntriesRequest(from, payload);
      break;

    case RESPOND_APPEND_ENTRY:
      this->handleAppendEntriesResponse(from, payload);
      break;

    case DISTRIBUTE_ENTRY:
      this->moveDataFromQueueToLog(from, payload);
      break;

    case DISTRIBUTE_ENTRY_ACK:
      this->handleAckFromLeaderQueue(from, payload);
      break;

    default:
      break;
  }
};

void _server::requestVote() {
  // Generate the message
  Message message(REQUEST_VOTE, this->_term);
  message.addFields(this->_log.getLastLogTerm(), this->_log.getLogSize());

  // Broadcast the message
  _mesh.sendBroadcast(message.serialize());

  this->_logger(DEBUG, "Requested vote from other nodes\n");
};

void _server::handleVoteRequest(uint32_t sender, DynamicJsonDocument& data) {
  // Equalize term with sender if term is lower
  if(this->_term < (uint32_t) data[TERM_FIELD_KEY]) {
    this->switchState(FOLLOWER, (uint32_t) data[TERM_FIELD_KEY]);
  }

  // Default vote grant to false
  bool granted = false;

  // If term is equal to sender && (haven't voted yet || voted for sender
  // before)
  // clang-format off
  if((this->_term == (uint32_t) data[TERM_FIELD_KEY]) &&
     (this->_voted_for == 0 || this->_voted_for == sender)) {

    // If log term and log size are greater than or equal to receiver's term and
    // size
    if((uint32_t) data[LAST_LOG_TERM_FIELD_KEY] >= this->_log.getLastLogTerm() &&
       (uint32_t) data[LAST_LOG_INDEX_FIELD_KEY] >= this->_log.getLogSize()) {

      // then vote for sender and reset alarm
      granted = true;
      this->_voted_for = sender;
      this->setElectionAlarmValue();
    }
  }
  // clang-format on

  // Generate the message
  Message message(SEND_VOTE, this->_term);
  message.addFields(granted);

  this->_mesh.sendMessageToNode(sender, message.serialize());

  this->_logger(DEBUG, "Replied to %u with %u vote\n", sender, granted);
};

void _server::handleVoteResponse(uint32_t sender, DynamicJsonDocument& data) {
  // Equalize term with responder if term is lower
  if(this->_term < (uint32_t) data[TERM_FIELD_KEY]) {
    this->switchState(FOLLOWER, (uint32_t) data[TERM_FIELD_KEY]);
  }

  // Update votes received map
  bool granted = data[GRANTED_FIELD_KEY];

  if(this->_state == CANDIDATE &&
     this->_term == (uint32_t) data[TERM_FIELD_KEY]) {
    // Store received vote in votes received map
    (*(this->_votes_received_ptr))[sender] = granted;
    this->_logger(DEBUG, "Saved vote from %u with %u vote\n", sender, granted);

    // Check for election results
    if(this->getElectionResults()) {
      this->_logger(DEBUG, "Won the election at %u\n", _mesh.getNodeTime());
      this->switchState(LEADER);
    } else {
      this->_logger(DEBUG,
                    "Did not win the election at %u\n",
                    this->_mesh.getNodeTime());
    }
  }
};

void _server::requestAppendEntries(uint32_t receiver, bool heart_beat) {
  // Generate the message
  Message message(REQUEST_APPEND_ENTRY, this->_term);

  uint32_t next_index = this->_log.getNextIndex(receiver);
  uint32_t previous_log_index = next_index - 1;
  uint32_t previous_log_term = this->_log.getLogTerm(previous_log_index);
  uint32_t commit_index = this->_commit_index;

  // Default to heart_beat message
  // Otherwise grab data from log
  string_t entries = HEART_BEAT_MESSAGE;
  if(!heart_beat &&
     (this->_log.getNextIndex(receiver) <= this->_log.getLogSize())) {
    entries = this->_log.getLogData(next_index);
  }

  message.addFields(previous_log_index,
                    previous_log_term,
                    entries,
                    commit_index);

  this->_mesh.sendMessageToNode(receiver, message.serialize());

  this->_logger(DEBUG, "Sent append entry request to %u\n", receiver);
};

void _server::broadcastRequestAppendEntries(bool heart_beat) {
  auto nodeList = this->_mesh.getNodeList(false);

  for(auto it = nodeList.begin(); it != nodeList.end(); ++it) {
    this->requestAppendEntries(*it, heart_beat);
  }

  this->_logger(DEBUG, "Broadcasted append entry request to everyone!\n");
}

void _server::handleAppendEntriesRequest(uint32_t sender,
                                         DynamicJsonDocument& data) {
  // Equalize term with sender if term is lower
  if(this->_term < (uint32_t) data[TERM_FIELD_KEY]) {
    this->switchState(FOLLOWER, (uint32_t) data[TERM_FIELD_KEY]);
  }

  // Rename variables from payload data
  auto previousLogIndex = (uint32_t) data[PREVIOUS_LOG_INDEX_FIELD_KEY];
  auto previousLogTerm = (uint32_t) data[PREVIOUS_LOG_TERM_FIELD_KEY];
  auto leaderCommit = (uint32_t) data[COMMIT_INDEX_FIELD_KEY];

  // TODO: If we want to be able to send multiple entries in the future,
  // create a function that can deserialize the JSON string received in the
  // entries field
  std::vector<std::pair<uint32_t, string_t>> received_entries;
  string_t entry_data = data[ENTRIES_FIELD_KEY];
  received_entries.push_back(
      std::make_pair((uint32_t) data[TERM_FIELD_KEY], entry_data));

  // Default message parameters
  Message message(RESPOND_APPEND_ENTRY, this->_term);

  bool message_success = false;
  uint32_t message_match_index = 0;

  if(data[ENTRIES_FIELD_KEY] == HEART_BEAT_MESSAGE) {
    this->_commit_index = std::max(leaderCommit, this->_commit_index);
    message_success = true;
    message_match_index = this->_commit_index;
    this->setElectionAlarmValue(5);
    this->_last_known_leader = sender;
  } else if(previousLogIndex == 0 ||
            (previousLogIndex <= this->_log.getLogSize() &&
             this->_log.getLogTerm(previousLogIndex) == previousLogTerm)) {
    message_success = true;

    auto loopIndex = previousLogIndex;

    for(uint32_t i = 0; i < received_entries.size(); i++) {
      ++loopIndex;
      if(this->_log.getLogTerm(loopIndex) != received_entries[i].first) {
        while(this->_log.getLogSize() > loopIndex) {
          this->_log.popEntry();
        }
        this->_log.pushEntry(received_entries[i]);
      }
    }

    message_match_index = loopIndex;

    this->_commit_index = std::max(leaderCommit, this->_commit_index);
  }

  message.addFields(message_success, message_match_index);

  this->_mesh.sendMessageToNode(sender, message.serialize());
  this->_logger(DEBUG, "Responded to append entry request from %u\n", sender);
};

void _server::handleAppendEntriesResponse(uint32_t sender,
                                          DynamicJsonDocument& data) {
  auto sender_term = (uint32_t) data[TERM_FIELD_KEY];
  auto success = (bool) data[SUCCESS_FIELD_KEY];
  auto sender_match_index = (uint32_t) data[MATCH_INDEX_FIELD_KEY];

  // Equalize term with sender if term is lower
  if(this->_term < sender_term) {
    this->switchState(FOLLOWER, sender_term);
  }

  if(this->_term == sender_term) {
    if(success) {
      this->_log.setMatchIndex(sender, sender_match_index);
      this->_log.setNextIndex(sender, sender_match_index + 1);
      this->_logger(
          DEBUG,
          "Set follower %u match index to %u since append entry succeeded\n",
          sender,
          sender_match_index);

    } else {
      this->_log.setNextIndex(
          sender,
          std::max((uint32_t) 1, this->_log.getNextIndex(sender) - 1));
      this->_logger(DEBUG,
                    "Decremented follower %u next index to %u since append "
                    "entry failed\n",
                    sender,
                    this->_log.getNextIndex(sender));
    }
  }
};

bool _server::distribute(string_t data, bool ack) {
  bool success = false;

  // TODO: Check for ack and push ack into the queue
  this->_data_queue.push(data);

  this->_logger(DEBUG, "I called distribute to share: \n");
  this->_logger(DEBUG, data);
  this->_logger(DEBUG, "\n");

  return ((ack == false) ? true : success);
};

void _server::moveDataFromQueueToLog(uint32_t sender,
                                     DynamicJsonDocument& data) {
  string_t received_data = data[DISTRIBUTE_ENTRY_KEY];
  bool send_ack = data[DISTRIBUTE_ENTRY_SEND_ACK_KEY];

  // Use leader's term instead of sender term
  this->_log.pushEntry(std::make_pair(this->_term, received_data));

  if(send_ack) {
    // Generate the message
    Message message(DISTRIBUTE_ENTRY_ACK, this->_term);
    // TODO: Grab real message id once we have it
    message.addFields(666, true);
    this->_mesh.sendMessageToNode(sender, message.serialize());
  }

  this->_logger(DEBUG, "I moved data from my queue to  my log: \n");
  this->_logger(DEBUG, received_data);
  this->_logger(DEBUG, "\n");
};

void _server::sendLocalQueueDataToLeaderQueue() {
  // Pop from data queue only if it is not empty
  if(!(this->_data_queue.checkEmpty())) {
    if(this->_state == LEADER) {
      string_t data = this->_data_queue.pop();

      // Push to own log
      this->_log.pushEntry(std::make_pair(this->_term, data));
      this->_logger(DEBUG,
                    "I sent data from my local queue to my own log, since I'm "
                    "the beloved leader\n");
    } else if(this->_last_known_leader != INFINITY) {
      string_t data = this->_data_queue.pop();

      // Generate the message
      Message message(DISTRIBUTE_ENTRY, this->_term);
      // TODO: read ack/nack from data queue
      message.addFields(data, false);
      this->_mesh.sendMessageToNode(this->_last_known_leader,
                                    message.serialize());
      this->_logger(
          DEBUG,
          "I sent data from my local queue to my beloved leader's queue\n");
    }
  }
};

void _server::handleAckFromLeaderQueue(uint32_t sender,
                                       DynamicJsonDocument& data) {
  // TODO: parse the message and then send ack/nack result to callback
  // function
}
