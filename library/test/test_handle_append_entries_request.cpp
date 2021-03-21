#include <string>

#include "catch2/catch.hpp"
#include "message.hpp"
#include "server.hpp"

using Catch::Matchers::Contains;

SCENARIO("Test server's handleAppendEntriesRequest") {
  using namespace broth::server;
  using namespace broth::message;

  Server server;
  DynamicJsonDocument data(10000);

  // Prepare the data for the message
  uint32_t sender = random();
  uint32_t term = 10;
  uint32_t previousLogIndex = 0;
  uint32_t previousLogTerm = 1;
  uint32_t commit_index = 1;
  string_t entries = HEART_BEAT_MESSAGE;

  data[TYPE_FIELD_KEY] = REQUEST_APPEND_ENTRY;
  data[TERM_FIELD_KEY] = term;
  data[PREVIOUS_LOG_INDEX_FIELD_KEY] = previousLogIndex;
  data[PREVIOUS_LOG_TERM_FIELD_KEY] = previousLogTerm;
  data[COMMIT_INDEX_FIELD_KEY] = commit_index;
  data[ENTRIES_FIELD_KEY] = entries;

  // Prepare server's internal data values
  server._term = 0;
  server._voted_for = 0;

  // Feed the data into target function
  server.handleAppendEntriesRequest(sender, data);

  // Test checks
  // REQUIRE(server._state == FOLLOWER);
  // REQUIRE(server._term == term);
  // REQUIRE(server._voted_for == 0);
}
