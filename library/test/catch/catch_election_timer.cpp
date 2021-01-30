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

  Server ramen;

  ramen._mesh.setMeshTime(100);
  logger(INFO, "Current mesh time is set to %u \n", ramen._mesh.getMeshTime());

  ramen.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);

  ramen.setElectionAlarm();
}
