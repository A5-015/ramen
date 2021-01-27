#define CATCH_CONFIG_MAIN

#define MESH_NAME "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT 5555

#include "Arduino.h"
#include "catch2/catch.hpp"
#include "ramen/logger.hpp"
#include "ramen/server.hpp"

ramen::logger::Logger ramen_logger;
using namespace ramen::server;

SCENARIO("Testing start of election") {
  ramen_logger.setLogLevel(ramen::server::DEBUG);
  Server consensus_on_mesh;
  // consensus_on_mesh.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
}
