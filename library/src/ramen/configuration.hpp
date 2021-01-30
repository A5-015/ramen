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

        #define TASK_ELECTION_INTERVAL 1000
        #define TASK_ELECTION_MAXIMUM_SKIPPED_COIN_FLIPS 20

    #endif

#else

    // Used for testing only
    #include <catch_configuration.hpp>

#endif
// clang-format on