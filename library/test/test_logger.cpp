#include "catch2/catch.hpp"
#include "logger.hpp"

SCENARIO("Logging test all levels") {
  using namespace broth::logger;
  Logger logger;

  logger.setLogLevel(DEBUG);

  logger(DEBUG, "Debug level message\n");
  logger(INFO, "Info level message\n");
  logger(WARNING, "Warning level message\n");
  logger(ERROR, "Error level message\n");
  logger(CRITICAL, "Critical level message\n"); 
}

SCENARIO("Logging level test") {
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
