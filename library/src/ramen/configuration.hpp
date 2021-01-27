#ifndef _RAMEN_CONFIGURATION_HPP_
#define _RAMEN_CONFIGURATION_HPP_

#include <Arduino.h>

#ifndef _RAMEN_CATCH_TESTING_
// painlessMesh handles correct version of TaskScheduler library
// #include <TaskScheduler.h>
#include <painlessMesh.h>
#endif

#define TASK_ELECTION_INTERVAL 1000
typedef String string_t;

#endif
