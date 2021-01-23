#define CATCH_CONFIG_MAIN

#include <Arduino.h>

#include "catch2/catch.hpp"
#include "ramen/logger.hpp"

ramen::Logger Log;

SCENARIO("Logging test") {
  Log.setLogLevel(ramen::ERROR);
  Log(ramen::ERROR,
      "This message should be visible and there shouldn't be a debug message "
      "below\n");
  Log(ramen::DEBUG, "This message should NOT be visible\n");
  Log(ramen::ERROR,
      "This message should also be visible and a CRITICAL message should be "
      "visible below\n");
  Log(ramen::CRITICAL, "This message should be visible\n");
}
