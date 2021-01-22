#define CATCH_CONFIG_MAIN

#include <Arduino.h>

#include "catch2/catch.hpp"
#include "ramen/logger.hpp"
using namespace ramen::logger;

LogClass Log;

SCENARIO("We can log things") {
  Log.setLogLevel(ERROR | DEBUG | COMMUNICATION);
  Log(ERROR, "We should see the next %u lines\n", 3);
  Log(DEBUG, "We should see the next %u lines\n", 2);
  Log(COMMUNICATION, "We should see the next %u lines\n", 1);
  Log(ERROR, "But not the next one\n");
  Log(S_TIME, "This should not be showing\n");
}
