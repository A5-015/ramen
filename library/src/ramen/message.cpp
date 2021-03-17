/**
 * @file message.cpp
 * @brief message.cpp
 *
 */
#include "ramen/message.hpp"

using _message = broth::message::Message;
using namespace broth::logger;

_message::Message(MessageType type, uint32_t term) {
  this->_message_type = type;
  this->_term = term;
};

string_t _message::serialize() {
  // Create JSON document
  StaticJsonDocument<1000> payload;

  // Dump the common fields
  payload["type"] = this->_message_type;
  payload["term"] = this->_term;

  // Dump integer fields
  for(auto it = this->_field_uint32_t.begin();
      it != this->_field_uint32_t.end();
      ++it) {
    payload[it->first] = it->second;
  }

  // Dump bool fields
  for(auto it = this->_field_bool.begin(); it != this->_field_bool.end();
      ++it) {
    payload[it->first] = it->second;
  }

  // Dump string fields
  for(auto it = this->_field_string_t.begin();
      it != this->_field_string_t.end();
      ++it) {
    payload[it->first] = it->second;
  }

  // Empty serialized payload
  string_t serialized_payload;

  // Fill the serialized payload
  serializeJson(payload, serialized_payload);

  return serialized_payload;
};
