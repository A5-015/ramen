/**
 * Override the configuration file for ramen
 **/

#ifndef _RAMEN_CONFIGURATION_HPP_
#define _RAMEN_CONFIGURATION_HPP_

// Used for testing of pc
typedef std::string string_t;

// TaskScheduler
#define _TASK_PRIORITY
#define _TASK_STD_FUNCTION
#include <TaskScheduler.h>

// ArduinoJson
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#undef ARDUINOJSON_ENABLE_ARDUINO_STRING

// clang-format off
#include "Arduino.h"
#include "fake_serial.hpp"
#include "fake_painlessmesh.hpp"
#include "fake_scheduler.hpp"
// clang-format on

// ramen variables
#define TASK_ELECTION_INTERVAL 1000

#endif
