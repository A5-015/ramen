#include "catch2/catch.hpp"
#include "ramen.h"

SCENARIO("Logging test") {
  using namespace broth::logger;
  Logger logger;

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
