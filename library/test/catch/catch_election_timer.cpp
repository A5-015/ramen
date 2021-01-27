#define CATCH_CONFIG_MAIN

#define MESH_NAME "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT 5555

#include "catch2/catch.hpp"
#include "catch_configuration.hpp"
#include "ramen/logger.hpp"
#include "ramen/server.hpp"

using namespace ramen::logger;
using namespace ramen::server;
Logger Log;

SCENARIO("Testing start of election") {
  Log.setLogLevel(ramen::server::DEBUG);
  Server consensus_on_mesh;
  // consensus_on_mesh.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
}
