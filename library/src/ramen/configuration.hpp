/**
 * @file configuration.hpp
 * @brief configuration.hpp
 *
 */
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

        #define ELECTION_TIMEOUT_FACTOR 100000
        #define MESSAGE_REQUEST_APPEND_DATA_ENTRY_SIZE 100

        #define HEART_BEAT_MESSAGE "__heart_beat__"

    #endif

#else

    // Used for testing only
    #include <catch_configuration.hpp>

#endif
// clang-format on