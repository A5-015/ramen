/**
 * Override the configuration file for ramen
 **/

// clang-format off
#ifndef _RAMEN_CONFIGURATION_HPP_

    #define _RAMEN_CONFIGURATION_HPP_

    #include "Arduino.h"
    #include <string>

    // Used for testing of pc
    typedef std::string string_t;

    // TaskScheduler
    #define _TASK_PRIORITY
    #define _TASK_STD_FUNCTION
    #include <TaskSchedulerDeclarations.h>

    // ArduinoJson
    #define ARDUINOJSON_USE_LONG_LONG 1
    #include <ArduinoJson.h>
    #undef ARDUINOJSON_ENABLE_ARDUINO_STRING

    // Arduino's Serial
    #include "fake_serial.hpp"

    // painlessMesh
    #include "fake_painlessmesh.hpp"
    using painlessMesh = fake_painlessmesh::painlessMesh;

    #include "catch_common.hpp"

    // Make everything public for testing
    #define private public

    // ramen variables
    #define TASK_ELECTION_INTERVAL 1000

#endif
// clang-format on