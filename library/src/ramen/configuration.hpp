/**
 * @file configuration.hpp
 * @brief configuration.hpp
 *
 */

//////////////////////////
// Place constants here //
// vvvvvvvvvvvvvvvvvvvv //

// In microseconds
#define RAFT_TIMER_PERIOD           100000
#define REQUEST_VOTE_TIMER_PERIOD   RAFT_TIMER_PERIOD * 2
#define HEART_BEAT_TIMER_PERIOD     RAFT_TIMER_PERIOD * 5
#define REQUEST_APPEND_ENTRY_PERIOD HEART_BEAT_TIMER_PERIOD * 3

#define ELECTION_TIMEOUT_FACTOR 100000

#define HEART_BEAT_MESSAGE "__heart_beat__"

// Message buffer sizes
// Check https://arduinojson.org/v6/assistant/ to figure out the right payload
// buffer size
// MAKE SURE TO UN-CHECK "Assume keys are const char*"
#define MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE 100
#define REQUEST_VOTE_SIZE                      130
#define SEND_VOTE_SIZE                         96
#define REQUEST_APPEND_ENTRY_SIZE              96 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE
#define RESPOND_APPEND_ENTRY_SIZE              96 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE
#define ENTRY_SIZE                             200 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE
#define DISTRIBUTE_ENTRY_SIZE                  100 + MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE
#define DISTRIBUTE_ENTRY_ACK_SIZE              96

// Text for message fields, these values will be used during JSON serialization
#define TYPE_FIELD_KEY                "type"
#define TERM_FIELD_KEY                "term"
#define LAST_LOG_TERM_FIELD_KEY       "lastLogTerm"
#define LAST_LOG_INDEX_FIELD_KEY      "lastLogIndex"
#define GRANTED_FIELD_KEY             "granted"
#define PREVIOUS_LOG_INDEX_FIELD_KEY  "previousLogIndex"
#define PREVIOUS_LOG_TERM_FIELD_KEY   "previousLogTerm"
#define ENTRIES_FIELD_KEY             "entries"
#define COMMIT_INDEX_FIELD_KEY        "commitIndex"
#define SUCCESS_FIELD_KEY             "success"
#define MATCH_INDEX_FIELD_KEY         "matchIndex"
#define DISTRIBUTE_ENTRY_KEY          "distrib"
#define DISTRIBUTE_ENTRY_SEND_ACK_KEY "distribSendAck"
#define DISTRIBUTE_ENTRY_ACK_KEY      "distribAck"

// ^^^^^^^^^^^^^^^^^^^^ //
//////////////////////////

// clang-format off
#ifndef _RAMEN_UNIT_TESTING_

    // Used for the actual library
    #ifndef _RAMEN_CONFIGURATION_HPP_
    #define _RAMEN_CONFIGURATION_HPP_

        #include <Arduino.h>

        // painlessMesh handles correct version of TaskScheduler library
        // #include <TaskScheduler.h>
        #include <painlessMesh.h>

        // NOLINTNEXTLINE
        typedef String string_t;

    #endif

#else

    // Used for testing only
    #include <catch_configuration.hpp>

#endif
// clang-format on