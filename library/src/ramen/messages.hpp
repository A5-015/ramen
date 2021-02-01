/**
 * @file messages.hpp
 * @brief messages.hpp
 * 
 */
#ifndef _RAMEN_MESSAGES_HPP_
#define _RAMEN_MESSAGES_HPP_

#include "ramen/configuration.hpp"

// Check https://arduinojson.org/v6/assistant/ to figure out the right payload
// buffer size

typedef enum { REQUEST_VOTE = 0, SEND_VOTE = 1 } MessageType;

class MessageRequestVote {
 public:
  MessageType type = REQUEST_VOTE;
  uint32_t term;
  uint32_t last_log_term;
  uint32_t last_log_index;

  string_t serialize() {
    DynamicJsonDocument payload(64);

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
    DynamicJsonDocument payload(48);

    string_t serialized_payload;
    payload["type"] = type;
    payload["term"] = term;
    payload["granted"] = granted;

    serializeJson(payload, serialized_payload);

    return serialized_payload;
  }
};

#endif