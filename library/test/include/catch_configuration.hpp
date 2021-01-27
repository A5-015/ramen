/**
 * Override the configuration file for ramen
 **/

#ifndef _RAMEN_CONFIGURATION_HPP_
#define _RAMEN_CONFIGURATION_HPP_

// clang-format off
#include "fake_arduino.h"
#include "fake_serial.hpp"
#include "fake_painlessmesh.hpp"
// clang-format on

#define _TASK_PRIORITY
#define _TASK_STD_FUNCTION

#include <TaskSchedulerDeclarations.h>

#define _RAMEN_CATCH_TESTING_

typedef std::string string_t;

#endif
