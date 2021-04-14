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
    #define ARDUINOJSON_ENABLE_STD_STRING 1
    #define ARDUINOJSON_ENABLE_ARDUINO_STRING 0
    #include <ArduinoJson.h>

    // Arduino's Serial
    #include "fake_serial.hpp"

    // painlessMesh
    #include "fake_painlessmesh.hpp"
    using painlessMesh = fake_painlessmesh::painlessMesh;

    #include "catch_common.hpp"

    // Make everything public for testing
    #define private public

    // Variavles for testing
    #define RAMEN_UNIT_TESTING_PAYLOAD_SIZE 100000

    // ramen variables to overwrite

#endif
// clang-format on