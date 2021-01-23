#include "ramen/server.hpp"

#include <Arduino.h>

void ramen::Server::switchMode(uint8_t mode){};
uint8_t ramen::Server::getMode(){};

void ramen::Server::setElectionAlarm(){};
void ramen::Server::startNewElection_candidate(){};
bool ramen::Server::getElectionResults(){};

void ramen::Server::broadcastData(std::string data){};
void ramen::Server::sendData(){};
std::string ramen::Server::receiveData(){};

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