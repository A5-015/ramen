/**
 * @file message.hpp
 * @brief message.hpp
 *
 */
#ifndef _RAMEN_MESSAGE_HPP_
#define _RAMEN_MESSAGE_HPP_

#include <cassert>
#include <map>

#include "ramen/configuration.hpp"
#include "ramen/logger.hpp"

namespace broth {
namespace message {
  /**
   * @brief Message type mapping
   *
   */
  typedef enum {
    REQUEST_VOTE = 0,
    SEND_VOTE = 1,
    REQUEST_APPEND_ENTRY = 2,
    RESPOND_APPEND_ENTRY = 3,
    ENTRY = 4
  } MessageType;

  // Check https://arduinojson.org/v6/assistant/ to figure out the right payload
  // buffer size

  //   /**
  //    * @brief Message fields available
  //    *
  //    */
  //   enum {
  //     LAST_LOG_TERM = 0,
  //     LAST_LOG_INDEX = 1,
  //     GRANTED = 2,
  //     PREVIOUS_LOG_INDEX = 3,
  //     PREVIOUS_LOG_TERM = 4,
  //     ENTRIES = 5,
  //     COMMIT_INDEX = 6,
  //     SUCCESS = 7,
  //     MATCH_INDEX = 8
  //   } MessageFields;

  /**
   * @brief Holds the required message size mapping
   *
   */
  typedef enum {
    REQUEST_VOTE_SIZE = 64,
    SEND_VOTE_SIZE = 48,
    REQUEST_APPEND_ENTRY_SIZE = 96 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE,
    RESPOND_APPEND_ENTRY_SIZE = 64 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE,
    ENTRY_SIZE = 100 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE
  } MessageSize;

  /**
   * @brief Class that is used to create messages that will be sent between mesh
   * nodes and serializing these messages
   *
   */
  class Message {
   private:
    string_t _serialized_payload;
    MessageType _message_type;
    uint32_t _term;

    std::map<string_t, uint32_t> _field_uint32_t;
    std::map<string_t, bool> _field_bool;
    std::map<string_t, string_t> _field_string_t;

   public:
    /**
     * @brief Construct a new Message object
     *
     * @param type Message type to initialize
     * @param term Current term of the node
     */
    Message(MessageType type, uint32_t term);

    /**
     * @brief MessageRequestVote
     *
     * @param last_log_term
     * @param last_log_index
     */
    void addFields(uint32_t last_log_term, uint32_t last_log_index) {
      assert(this->_message_type == REQUEST_VOTE);
      // clang-format off
      this->_field_uint32_t.insert(std::make_pair("lastLogTerm", last_log_term));
      this->_field_uint32_t.insert(std::make_pair("lastLogIndex", last_log_index));
      // clang-format on
    };

    /**
     * @brief MessageSendVote
     *
     * @param granted
     */
    void addFields(bool granted) {
      assert(this->_message_type == SEND_VOTE);
      this->_field_bool.insert(std::make_pair("granted", granted));
    };

    /**
     * @brief MessageRequestAppendEntry
     *
     * @param previous_log_index
     * @param previous_log_term
     * @param entries
     * @param commit_index
     */
    void addFields(uint32_t previous_log_index,
                   uint32_t previous_log_term,
                   string_t entries,
                   uint32_t commit_index) {
      assert(this->_message_type == REQUEST_APPEND_ENTRY);
      // clang-format off
      this->_field_uint32_t.insert(std::make_pair("previousLogIndex", previous_log_index));
      this->_field_uint32_t.insert(std::make_pair("previousLogTerm", previous_log_term));
      this->_field_string_t.insert(std::make_pair("entries", entries));
      this->_field_uint32_t.insert(std::make_pair("commitIndex", commit_index));
      // clang-format on
    };

    /**
     * @brief MessageRespondAppendEntry
     *
     * @param success
     * @param match_index
     */
    void addFields(bool success, uint32_t match_index) {
      assert(this->_message_type == RESPOND_APPEND_ENTRY);
      this->_field_bool.insert(std::make_pair("success", success));
      this->_field_uint32_t.insert(std::make_pair("matchIndex", match_index));
    };

    /**
     * @brief Serializes the current message object to a JSON string
     *
     * @return string_t
     */
    string_t serialize();
  };
} // namespace message
} // namespace broth

#endif