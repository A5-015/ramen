#include "ramen/server.hpp"

#include <Arduino.h>

using namespace ramen::server;
using namespace ramen::dataqueue;
using namespace ramen::logholder;
using namespace ramen::logger;

Server::Server() { Log.setLogLevel(DEBUG); };

void Server::init(std::string meshName, std::string meshPassword,
                  uint8_t meshPort) {
  Log(DEBUG, "Just initialized the system\n");
};

void Server::update(){};

void Server::switchState(uint8_t state){};
uint8_t Server::getState() { return Server::state; };

void Server::setElectionAlarm(){};
void Server::startNewElection(){};
bool Server::getElectionResults() { return false; };

void Server::broadcastData(std::string data){};
void Server::sendData(uint32_t receiver, std::string data){};
std::string Server::receiveData() {
  std::string data;
  return data;
};

void Server::requestVote(uint32_t receiver, std::string data){};
void Server::handleVoteRequest(uint32_t sender, std::string data){};
void Server::handleVoteResponse(uint32_t sender, std::string data){};

void Server::requestAppendEntries(uint32_t receiver, std::string data){};
void Server::handleAppendEntriesRequest(uint32_t sender, std::string data){};
void Server::handleAppendEntriesResponse(){};

void Server::moveDataFromQueueToLog(DataQueue queue, LogHolder log){};
void Server::sendLocalQueueDataToLeaderQueue(std::string data){};
void Server::handleNewData(std::string data){};