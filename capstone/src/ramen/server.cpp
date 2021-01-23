#include "ramen/server.hpp"

#include <Arduino.h>

ramen::Server::Server(){};

void ramen::Server::init(std::string meshName, std::string meshPassword,
                         uint8_t meshPort){

};

void ramen::Server::update(){};

void ramen::Server::switchState(uint8_t state){};
uint8_t ramen::Server::getState() { return ramen::Server::state; };

void ramen::Server::setElectionAlarm(){};
void ramen::Server::startNewElection(){};
bool ramen::Server::getElectionResults() { return false; };

void ramen::Server::broadcastData(std::string data){};
void ramen::Server::sendData(uint32_t receiver, std::string data){};
std::string ramen::Server::receiveData() {
  std::string data;
  return data;
};

void ramen::Server::requestVote(uint32_t receiver, std::string data){};
void ramen::Server::handleVoteRequest(uint32_t sender, std::string data){};
void ramen::Server::handleVoteResponse(uint32_t sender, std::string data){};

void ramen::Server::requestAppendEntries(uint32_t receiver, std::string data){};
void ramen::Server::handleAppendEntriesRequest(uint32_t sender,
                                               std::string data){};
void ramen::Server::handleAppendEntriesResponse(){};

void ramen::Server::moveDataFromQueueToLog(DataQueue queue, LogHolder log){};
void ramen::Server::sendLocalQueueDataToLeaderQueue(std::string data){};
void ramen::Server::handleNewData(std::string data){};