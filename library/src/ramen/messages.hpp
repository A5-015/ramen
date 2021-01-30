#ifndef _RAMEN_MESSAGES_HPP_
#define _RAMEN_MESSAGES_HPP_

#include "ramen/configuration.hpp"

// Check https://arduinojson.org/v6/assistant/ to figure out the right payload
// buffer size

class MessageRequestVote {
 public:
  const string_t type = "RequestVote";
  uint32_t term;
  uint32_t last_log_term;
  uint32_t last_log_index;

  string_t serialize() {
    DynamicJsonDocument payload(96);

    string_t serialized_payload;
    payload["type"] = type;
    payload["term"] = term;
    payload["lastLogTerm"] = last_log_term;
    payload["lastLogIndex"] = last_log_index;

    serializeJson(payload, serialized_payload);

    return serialized_payload;
  }
};

#endif