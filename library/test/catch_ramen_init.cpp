#include "catch2/catch.hpp"
#include "ramen.h"

SCENARIO("Check if ramen initializes correctly") {
  ramenServer network;

  // Assign node id for testing
  network._mesh.setNodeId(12345);

  network.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);

  REQUIRE(network._state == ramen::server::FOLLOWER);
  REQUIRE(network._term == 0);
  REQUIRE(network._id == 12345);
  REQUIRE(network._received_new_append_entry_request == false);
}
