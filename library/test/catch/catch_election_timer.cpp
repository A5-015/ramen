#define CATCH_CONFIG_MAIN

#define MESH_NAME "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT 5555

#include "catch2/catch.hpp"
#include "ramen.h"

using namespace ramen::logger;
using namespace ramen::server;
Logger Log;

SCENARIO("Testing start of election") {
  Log.setLogLevel(DEBUG);

  Server ramen;

  ramen.mesh.setMeshTime(100);
  Log(INFO, "Current mesh time is set to %u \n", ramen.mesh.getMeshTime());

  ramen.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
}
