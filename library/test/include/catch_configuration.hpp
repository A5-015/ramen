/**
 * Override the configuration file for ramen
 **/

// clang-format off
#ifndef _RAMEN_CONFIGURATION_HPP_
#define _RAMEN_CONFIGURATION_HPP_

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

    #include "Arduino.h"
    #include "fake_serial.hpp"
    #include "fake_painlessmesh.hpp"

    // ramen variables
    #define TASK_ELECTION_INTERVAL 1000

#endif
// clang-format on