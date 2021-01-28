#include "ramen/server.hpp"

using _server = ramen::server::Server;
using _dataqueue = ramen::dataqueue::DataQueue;
using _logholder = ramen::logholder::LogHolder;
using namespace ramen::logger;

void testFunction() {
  Logger ServerLogger;
  ServerLogger.setLogLevel(ramen::logger::LogLevel::DEBUG);
  ServerLogger(ramen::logger::LogLevel::DEBUG, "poop\n");
}

Task testingTask(TASK_SECOND * 1, TASK_FOREVER, testFunction);

_server::Server() : state(0), term(0) {
  // Set logging level to DEBUG
  Log.setLogLevel(DEBUG);

  // TODO: Change to switchState()
};

void _server::init(string_t meshName, string_t meshPassword,
                   uint16_t meshPort) {
  // Initialize painlessMesh
  mesh.init(meshName, meshPassword, &scheduler, meshPort);
  Log(DEBUG, "Just initialized painlessMesh\n");
  id = mesh.getNodeId();
};

void _server::update() { mesh.update(); };

void _server::switchState(uint8_t state){};
uint8_t _server::getState() { return _server::state; };

void _server::setElectionAlarm() {
  // Task election(TASK_ELECTION_INTERVAL * TASK_MILLISECOND, TASK_FOREVER, [&]
  // {
  //   if (true) {
  //     // If coin toss results in 1, start a new election
  //     // In this way, we randomize the election duration without worrying
  //     // about timer overflow
  //     Log(DEBUG, "Should start an election now!");
  //     startNewElection();
  //   }
  // });
  scheduler.addTask(testingTask);
  testingTask.enable();
  Log(DEBUG, "Set the election timer\n");
};

void _server::startNewElection() { Log(INFO, "Started election!"); };
bool _server::getElectionResults() { return false; };

void _server::broadcastData(string_t data){};
void _server::sendData(uint32_t receiver, string_t data){};
string_t _server::receiveData() {
  string_t data;
  return data;
};

void _server::requestVote(uint32_t receiver, string_t data){};
void _server::handleVoteRequest(uint32_t sender, string_t data){};
void _server::handleVoteResponse(uint32_t sender, string_t data){};

void _server::requestAppendEntries(uint32_t receiver, string_t data){};
void _server::handleAppendEntriesRequest(uint32_t sender, string_t data){};
void _server::handleAppendEntriesResponse(){};

void _server::moveDataFromQueueToLog(DataQueue queue, LogHolder log){};
void _server::sendLocalQueueDataToLeaderQueue(string_t data){};
void _server::handleNewData(string_t data){};