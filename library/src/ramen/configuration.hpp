/**
 * @file configuration.hpp
 * @brief configuration.hpp
 *
 */

//////////////////////////
// Place constants here //
// vvvvvvvvvvvvvvvvvvvv //

#define ELECTION_TIMEOUT_FACTOR                100000
#define MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE 100
#define HEART_BEAT_MESSAGE                     "__heart_beat__"

// Text for message fields, these values will be used during JSON serialization
#define TYPE_FIELD_KEY               "type"
#define TERM_FIELD_KEY               "term"
#define LAST_LOG_TERM_FIELD_KEY      "lastLogTerm"
#define LAST_LOG_INDEX_FIELD_KEY     "lastLogIndex"
#define GRANTED_FIELD_KEY            "granted"
#define PREVIOUS_LOG_INDEX_FIELD_KEY "previousLogIndex"
#define PREVIOUS_LOG_TERM_FIELD_KEY  "previousLogTerm"
#define ENTRIES_FIELD_KEY            "entries"
#define COMMIT_INDEX_FIELD_KEY       "commitIndex"
#define SUCCESS_FIELD_KEY            "success"
#define MATCH_INDEX_FIELD_KEY        "matchIndex"

// ^^^^^^^^^^^^^^^^^^^^ //
//////////////////////////

// clang-format off
#ifndef _RAMEN_CATCH_TESTING_

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