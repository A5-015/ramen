#include <string>

#include "catch2/catch.hpp"
#include "server.hpp"

using Catch::Matchers::Contains;

SCENARIO("Test server's switch state") {
  GIVEN("Initialized server object") {
    using namespace broth::server;
    Server server;
    uint32_t term = random();

    WHEN("We want to switch to FOLLOWER state") {
      server.switchState(FOLLOWER, term);

      REQUIRE(server._state == FOLLOWER);
      REQUIRE(server._term == term);
      REQUIRE(server._voted_for == 0);
    }

    WHEN("We want to switch to CANDIDATE state") {
      server.switchState(CANDIDATE, term);

      // TODO: understand why we don't do anything for this in switch state
      // implementation
    }

    WHEN("We want to switch to LEADER state") {
      server.switchState(LEADER, term);

      REQUIRE(server._state == LEADER);
    }
  }
}
