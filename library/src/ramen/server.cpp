#include "ramen/server.hpp"

#include <Arduino.h>
#include <painlessMesh.h>

using _server = ramen::server::Server;
using _dataqueue = ramen::dataqueue::DataQueue;
using _logholder = ramen::logholder::LogHolder;
using namespace ramen::logger;

_server::Server() : state(0), term(0) {
  // Set logging level to DEBUG
  Log.setLogLevel(DEBUG);

  // TODO: Change to switchState()
};

void _server::init(std::string meshName, std::string meshPassword,
                   uint8_t meshPort) {
  Log(DEBUG, "Just initialized the system\n");
};

void _server::update(){};

void _server::switchState(uint8_t state){};
uint8_t _server::getState() { return _server::state; };

void _server::setElectionAlarm(){};
void _server::startNewElection(){};
bool _server::getElectionResults() { return false; };

void _server::broadcastData(std::string data){};
void _server::sendData(uint32_t receiver, std::string data){};
std::string _server::receiveData() {
  std::string data;
  return data;
};

void _server::requestVote(uint32_t receiver, std::string data){};
void _server::handleVoteRequest(uint32_t sender, std::string data){};
void _server::handleVoteResponse(uint32_t sender, std::string data){};

void _server::requestAppendEntries(uint32_t receiver, std::string data){};
void _server::handleAppendEntriesRequest(uint32_t sender, std::string data){};
void _server::handleAppendEntriesResponse(){};

void _server::moveDataFromQueueToLog(DataQueue queue, LogHolder log){};
void _server::sendLocalQueueDataToLeaderQueue(std::string data){};
void _server::handleNewData(std::string data){};