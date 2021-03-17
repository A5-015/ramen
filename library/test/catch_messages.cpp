#include <string>

#include "catch2/catch.hpp"
#include "message.hpp"

using Catch::Matchers::Contains;

SCENARIO(
    "Testing different message types with randomized values to check if "
    "serialization works") {
  GIVEN("Message namespace") {
    using namespace broth::message;
    uint32_t term = random();

    WHEN("REQUEST_VOTE is used properly") {
      uint32_t last_log_term = random();
      uint32_t last_log_index = random();

      // Create the message
      Message message(REQUEST_VOTE, term);
      message.addFields(last_log_term, last_log_index);

      // Serialize the message
      string_t serialized = message.serialize();

      // Check for JSON keys
      REQUIRE_THAT(serialized, Contains("type"));
      REQUIRE_THAT(serialized, Contains("term"));
      REQUIRE_THAT(serialized, Contains("lastLogTerm"));
      REQUIRE_THAT(serialized, Contains("lastLogIndex"));

      // Check for the key values
      REQUIRE_THAT(serialized, Contains(std::to_string(REQUEST_VOTE)));
      REQUIRE_THAT(serialized, Contains(std::to_string(term)));
      REQUIRE_THAT(serialized, Contains(std::to_string(last_log_term)));
      REQUIRE_THAT(serialized, Contains(std::to_string(last_log_index)));
    }

    WHEN("SEND_VOTE is used properly") {
      bool granted = random() % 2;

      // Create the message
      Message message(SEND_VOTE, term);
      message.addFields(granted);

      // Serialize the message
      string_t serialized = message.serialize();

      // Check for JSON keys
      REQUIRE_THAT(serialized, Contains("type"));
      REQUIRE_THAT(serialized, Contains("term"));
      REQUIRE_THAT(serialized, Contains("granted"));

      // Check for the key values
      REQUIRE_THAT(serialized, Contains(std::to_string(SEND_VOTE)));
      REQUIRE_THAT(serialized, Contains(std::to_string(term)));
      REQUIRE_THAT(serialized, Contains(std::to_string(granted)));
    }

    WHEN("REQUEST_APPEND_ENTRY is used properly") {
      uint32_t previous_log_index = random();
      uint32_t previous_log_term = random();
      string_t entries = "test_string_for_entry";
      uint32_t commit_index = random();

      // Create the message
      Message message(REQUEST_APPEND_ENTRY, term);
      message.addFields(previous_log_index,
                        previous_log_term,
                        entries,
                        commit_index);

      // Serialize the message
      string_t serialized = message.serialize();

      // Check for JSON keys
      REQUIRE_THAT(serialized, Contains("type"));
      REQUIRE_THAT(serialized, Contains("term"));
      REQUIRE_THAT(serialized, Contains("previousLogIndex"));
      REQUIRE_THAT(serialized, Contains("previousLogTerm"));
      REQUIRE_THAT(serialized, Contains("entries"));
      REQUIRE_THAT(serialized, Contains("commitIndex"));

      // Check for the key values
      REQUIRE_THAT(serialized, Contains(std::to_string(REQUEST_APPEND_ENTRY)));
      REQUIRE_THAT(serialized, Contains(std::to_string(term)));
      REQUIRE_THAT(serialized, Contains(std::to_string(previous_log_index)));
      REQUIRE_THAT(serialized, Contains(std::to_string(previous_log_term)));
      REQUIRE_THAT(serialized, Contains(entries));
      REQUIRE_THAT(serialized, Contains(std::to_string(commit_index)));
    }

    WHEN("RESPOND_APPEND_ENTRY is used properly") {
      bool success = random() % 2;
      uint32_t match_index = random();

      // Create the message
      Message message(RESPOND_APPEND_ENTRY, term);
      message.addFields(success, match_index);

      // Serialize the message
      string_t serialized = message.serialize();

      // Check for JSON keys
      REQUIRE_THAT(serialized, Contains("type"));
      REQUIRE_THAT(serialized, Contains("term"));
      REQUIRE_THAT(serialized, Contains("success"));
      REQUIRE_THAT(serialized, Contains("matchIndex"));

      // Check for the key values
      REQUIRE_THAT(serialized, Contains(std::to_string(RESPOND_APPEND_ENTRY)));
      REQUIRE_THAT(serialized, Contains(std::to_string(term)));
      REQUIRE_THAT(serialized, Contains(std::to_string(success)));
      REQUIRE_THAT(serialized, Contains(std::to_string(match_index)));
    }
  }
}
