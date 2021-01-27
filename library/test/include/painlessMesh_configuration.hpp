/**
 * Override the configuration file for painlessMesh
 * Copied from
 *https://gitlab.com/painlessMesh/painlessMesh/-/blob/master/test/catch/Arduino.h
 * Refresh the file when needed from the source.
 **/

#ifndef _PAINLESS_MESH_CONFIGURATION_HPP_
#define _PAINLESS_MESH_CONFIGURATION_HPP_

#define _TASK_PRIORITY  // Support for layered scheduling priority
#define _TASK_STD_FUNCTION

#include <TaskSchedulerDeclarations.h>

#define ARDUINOJSON_USE_LONG_LONG 1

#include <ArduinoJson.h>
#undef ARDUINOJSON_ENABLE_ARDUINO_STRING

#define ICACHE_FLASH_ATTR

#define PAINLESSMESH_ENABLE_STD_STRING

// Enable OTA support
#define PAINLESSMESH_ENABLE_OTA

#define NODE_TIMEOUT 5 * TASK_SECOND

typedef std::string TSTRING;

#ifdef ESP32
#define MAX_CONN 10
#else
#define MAX_CONN 4
#endif  // DEBUG

#include "fake_asynctcp.hpp"
#include "fake_serial.hpp"

extern WiFiClass WiFi;
extern ESPClass ESP;

#endif
