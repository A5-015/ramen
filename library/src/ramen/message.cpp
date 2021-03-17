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

_message::~Message() {
  delete this->_payload;
};

string_t _message::serialize() {
  // We need a larger buffer due to data type differences on a x86 computer, so
  // we need to redefine a quite large buffer instead of the default ones for
  // ESP8266
#ifdef _RAMEN_CATCH_TESTING_
  delete this->_payload;
  this->_payload = new DynamicJsonDocument(RAMEN_CATCH_TESTING_PAYLOAD_SIZE);
#endif

  // Dump the common fields
  (*(this->_payload))[TYPE_FIELD_KEY] = this->_message_type;
  (*(this->_payload))[TERM_FIELD_KEY] = this->_term;

  // Dump integer fields
  for(auto it = this->_field_uint32_t.begin();
      it != this->_field_uint32_t.end();
      ++it) {
    (*(this->_payload))[it->first] = it->second;
  }

  // Dump bool fields
  for(auto it = this->_field_bool.begin(); it != this->_field_bool.end();
      ++it) {
    (*(this->_payload))[it->first] = it->second;
  }

  // Dump string fields
  for(auto it = this->_field_string_t.begin();
      it != this->_field_string_t.end();
      ++it) {
    (*(this->_payload))[it->first] = it->second;
  }

  // Empty serialized payload
  string_t serialized_payload;

  // Fill the serialized payload
  serializeJson((*(this->_payload)), serialized_payload);

  return serialized_payload;
};
