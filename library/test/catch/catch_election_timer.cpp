#define CATCH_CONFIG_MAIN

#define MESH_NAME "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT 5555

#include <Arduino.h>

#include "catch2/catch.hpp"
#include "ramen.h"

// using namespace ramen::logger;
ramen::logger::Logger Log;

SCENARIO("Testing start of election") {
  Log.setLogLevel(DEBUG);
  ramen::server::Server consensus_on_mesh;
  consensus_on_mesh.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
}
