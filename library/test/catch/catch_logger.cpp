#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"
#include "ramen.h"

using namespace ramen::logger;
Logger logger;

SCENARIO("Logging test") {
  logger.setLogLevel(ERROR);
  logger(
      ERROR,
      "This message should be visible and there shouldn't be a debug message "
      "below\n");
  logger(DEBUG, "This message should NOT be visible\n");
  logger(ERROR,
         "This message should also be visible and a CRITICAL message should be "
         "visible below\n");
  logger(CRITICAL, "This message should be visible\n");
}
