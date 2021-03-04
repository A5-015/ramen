/**
 * @file messages.hpp
 * @brief messages.hpp
 *
 */
#ifndef _RAMEN_MESSAGES_HPP_
#define _RAMEN_MESSAGES_HPP_

#define MESSAGE_REQUEST_APPEND_ENTRY_PAYLOAD_SIZE \
  96 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE

#define MESSAGE_RESPOND_APPEND_ENTRY_PAYLOAD_SIZE \
  64 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE

#include "ramen/configuration.hpp"

// Check https://arduinojson.org/v6/assistant/ to figure out the right payload
// buffer size

typedef enum {
  REQUEST_VOTE = 0,
  SEND_VOTE = 1,
  REQUEST_APPEND_ENTRY = 2,
  RESPOND_APPEND_ENTRY = 3,
  ENTRY = 4
} MessageType;

class MessageRequestVote {
 public:
  MessageType type = REQUEST_VOTE;
  uint32_t term;
  uint32_t last_log_term;
  uint32_t last_log_index;

  string_t serialize() {
#ifdef _RAMEN_CATCH_TESTING_
    DynamicJsonDocument payload(RAMEN_CATCH_TESTING_PAYLOAD_SIZE);
#else
    DynamicJsonDocument payload(64);
#endif

    string_t serialized_payload;
    payload["type"] = type;
    payload["term"] = term;
    payload["lastLogTerm"] = last_log_term;
    payload["lastLogIndex"] = last_log_index;

    serializeJson(payload, serialized_payload);

    return serialized_payload;
  }
};

class MessageSendVote {
 public:
  MessageType type = SEND_VOTE;
  uint32_t term;
  bool granted;

  string_t serialize() {
#ifdef _RAMEN_CATCH_TESTING_
    DynamicJsonDocument payload(RAMEN_CATCH_TESTING_PAYLOAD_SIZE);
#else
    DynamicJsonDocument payload(48);
#endif

    string_t serialized_payload;
    payload["type"] = type;
    payload["term"] = term;
    payload["granted"] = granted;

    serializeJson(payload, serialized_payload);

    return serialized_payload;
  }
};

class MessageRequestAppendEntry {
 public:
  MessageType type = REQUEST_APPEND_ENTRY;
  uint32_t term;
  uint32_t previous_log_index;
  uint32_t previous_log_term;
  string_t entries;
  uint32_t commit_index;

  string_t serialize() {
#ifdef _RAMEN_CATCH_TESTING_
    DynamicJsonDocument payload(RAMEN_CATCH_TESTING_PAYLOAD_SIZE);
#else
    DynamicJsonDocument payload(MESSAGE_REQUEST_APPEND_ENTRY_PAYLOAD_SIZE);
#endif

    string_t serialized_payload;
    payload["type"] = type;
    payload["term"] = term;
    payload["previousLogIndex"] = previous_log_index;
    payload["previousLogTerm"] = previous_log_term;
    payload["entries"] = entries;
    payload["commitIndex"] = commit_index;

    serializeJson(payload, serialized_payload);

    return serialized_payload;
  }
};

class MessageRespondAppendEntry {
 public:
  MessageType type = RESPOND_APPEND_ENTRY;
  uint32_t term;
  bool success;
  uint32_t match_index;

  string_t serialize() {
#ifdef _RAMEN_CATCH_TESTING_
    DynamicJsonDocument payload(RAMEN_CATCH_TESTING_PAYLOAD_SIZE);
#else
    DynamicJsonDocument payload(MESSAGE_REQUEST_APPEND_ENTRY_PAYLOAD_SIZE);
#endif

    string_t serialized_payload;
    payload["type"] = type;
    payload["term"] = term;
    payload["success"] = success;
    payload["match_index"] = match_index;

    serializeJson(payload, serialized_payload);

    return serialized_payload;
  }
};

// TODO: Draft
class MessageEntry {
 public:
  MessageType type = ENTRY;

  string_t serialize(std::vector<std::pair<uint32_t, string_t>> entries) {
#ifdef _RAMEN_CATCH_TESTING_
    DynamicJsonDocument payload(RAMEN_CATCH_TESTING_PAYLOAD_SIZE);
#else
    DynamicJsonDocument payload(MESSAGE_REQUEST_APPEND_ENTRY_PAYLOAD_SIZE);
#endif

    string_t serialized_payload;
    payload["type"] = type;
    payload["entries"] = entries;

    serializeJson(payload, serialized_payload);

    return serialized_payload;
  }
};

#endif