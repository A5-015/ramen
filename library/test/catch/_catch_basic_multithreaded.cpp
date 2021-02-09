#define CATCH_CONFIG_MAIN

#define MESH_NAME     "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT     5555

#include <thread>

#include "catch2/catch.hpp"
#include "ramen.h"

using namespace ramen::logger;
using namespace ramen::server;
Logger logger;

SCENARIO("Testing the operation of ramen using multithreading") {
  logger.setLogLevel(DEBUG);

  Server ramen_1, ramen_2, ramen_3;

  // Assign node ids
  ramen_1._mesh.setNodeId(1);
  ramen_2._mesh.setNodeId(2);
  ramen_3._mesh.setNodeId(3);

  // Initialize the nodes
  ramen_1.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
  ramen_2.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
  ramen_3.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);

  // Override node's internal logging levels
  ramen_1._logger.setLogLevel(DEBUG);
  ramen_2._logger.setLogLevel(DEBUG);
  ramen_3._logger.setLogLevel(DEBUG);

  // Create the connections between nodes
  ramen_1._mesh.addNeighbourNode(ramen_2._mesh);
  ramen_1._mesh.addNeighbourNode(ramen_3._mesh);

  ramen_2._mesh.addNeighbourNode(ramen_1._mesh);
  ramen_2._mesh.addNeighbourNode(ramen_3._mesh);

  ramen_3._mesh.addNeighbourNode(ramen_1._mesh);
  ramen_3._mesh.addNeighbourNode(ramen_2._mesh);

  // Simulates loop() from Arduino
  for(uint8_t i = 0; i < 10; ++i) {
    std::thread t1(&Server::update, &ramen_1);
    std::thread t2(&Server::update, &ramen_2);
    std::thread t3(&Server::update, &ramen_3);
    t1.join();
    t2.join();
    t3.join();
  }
}
