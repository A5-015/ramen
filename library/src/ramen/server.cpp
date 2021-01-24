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

void _server::init(String meshName, String meshPassword, uint16_t meshPort) {
  // Initialize painlessMesh
  mesh.init(meshName, meshPassword, &scheduler, meshPort);
  Log(DEBUG, "Just initialized painlessMesh\n");
  id = mesh.getNodeId();
};

void _server::update(){};

void _server::switchState(uint8_t state){};
uint8_t _server::getState() { return _server::state; };

void _server::setElectionAlarm() {
  Task election(TASK_ELECTION_INTERVAL * TASK_MILLISECOND, TASK_FOREVER, [&] {
    if (mesh.getNodeTime() % 2) {
      // If coin toss results in 1, start a new election
      // In this way, we randomize the election duration without worrying about
      // timer overflow
      startNewElection();
    }
  });
  scheduler.addTask(election);
  election.enable();
};

void _server::startNewElection() { Log(INFO, "Started election!"); };
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