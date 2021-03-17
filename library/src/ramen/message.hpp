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
    DynamicJsonDocument* _payload;

    std::map<string_t, uint32_t> _field_uint32_t;
    std::map<string_t, bool> _field_bool;
    std::map<string_t, string_t> _field_string_t;

   public:
    /**
     * @brief Destroy the Message object
     *
     */
    ~Message();

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

      // Initialize the correct size
      this->_payload = new DynamicJsonDocument(REQUEST_VOTE_SIZE);

      // clang-format off
      this->_field_uint32_t.insert(std::make_pair(LAST_LOG_TERM_FIELD_KEY, last_log_term));
      this->_field_uint32_t.insert(std::make_pair(LAST_LOG_INDEX_FIELD_KEY, last_log_index));
      // clang-format on
    };

    /**
     * @brief MessageSendVote
     *
     * @param granted
     */
    void addFields(bool granted) {
      assert(this->_message_type == SEND_VOTE);

      // Initialize the correct size
      this->_payload = new DynamicJsonDocument(SEND_VOTE_SIZE);

      this->_field_bool.insert(std::make_pair(GRANTED_FIELD_KEY, granted));
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

      // Initialize the correct size
      this->_payload = new DynamicJsonDocument(REQUEST_APPEND_ENTRY_SIZE);

      // clang-format off
      this->_field_uint32_t.insert(std::make_pair(PREVIOUS_LOG_INDEX_FIELD_KEY, previous_log_index));
      this->_field_uint32_t.insert(std::make_pair(PREVIOUS_LOG_TERM_FIELD_KEY, previous_log_term));
      this->_field_string_t.insert(std::make_pair(ENTRIES_FIELD_KEY, entries));
      this->_field_uint32_t.insert(std::make_pair(COMMIT_INDEX_FIELD_KEY, commit_index));
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

      // Initialize the correct size
      this->_payload = new DynamicJsonDocument(RESPOND_APPEND_ENTRY_SIZE);

      // clang-format off
      this->_field_bool.insert(std::make_pair(SUCCESS_FIELD_KEY, success));
      this->_field_uint32_t.insert(std::make_pair(MATCH_INDEX_FIELD_KEY, match_index));
      // clang-format on
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