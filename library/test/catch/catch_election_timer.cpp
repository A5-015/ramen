#define CATCH_CONFIG_MAIN

#define MESH_NAME     "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT     5555

#include "catch2/catch.hpp"
#include "ramen.h"

using namespace ramen::logger;
using namespace ramen::server;
Logger logger;

SCENARIO("Testing start of election") {
  logger.setLogLevel(DEBUG);

  Server ramen_1, ramen_2;

  // Assign node ids
  ramen_1._mesh.setNodeId(1);
  ramen_2._mesh.setNodeId(2);

  ramen_1.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
  ramen_2.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);

  // Create the connections between nodes
  ramen_1._mesh.addNeighbourNode(ramen_2._mesh);
  ramen_2._mesh.addNeighbourNode(ramen_1._mesh);

  // Simulates loop() from Arduino
  for(uint8_t i = 0; i < 50; ++i) {
    ramen_1.update();
    ramen_2.update();
  }
}
