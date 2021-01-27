// clang-format off
#ifndef _RAMEN_CATCH_TESTING_

    // Used for the actual library
    #ifndef _RAMEN_CONFIGURATION_HPP_
    
        #define _RAMEN_CONFIGURATION_HPP_

        #include <Arduino.h>

        // painlessMesh handles correct version of TaskScheduler library
        // #include <TaskScheduler.h>
        #include <painlessMesh.h>

        #define TASK_ELECTION_INTERVAL 1000
        typedef String string_t;

    #endif

#else

    // Used for testing
    #include <catch_configuration.hpp>

#endif
// clang-format on