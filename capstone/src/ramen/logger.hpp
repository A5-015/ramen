#ifndef _RAMEN_LOGGER_HPP_
#define _RAMEN_LOGGER_HPP_

#include <Arduino.h>
#include <stdarg.h>

namespace ramen {

typedef enum {
  DEBUG = 1,
  INFO = 2,
  WARNING = 3,
  ERROR = 4,
  CRITICAL = 5
} LogLevel;

/**
 * @brief The logger class for logging messages. Maximum message size is 200
 * characters.
 *
 */
class Logger {
 private:
  uint8_t level = 0;
  char str[200];

 public:
  /**
   * @brief Set the logging level
   *
   * @param givenLevel DEBUG or INFO or WARNING or ERROR or CRITICAL
   */
  void setLogLevel(uint8_t givenLevel) {
    level = givenLevel;

    Serial.print(F("Logging level is set to: "));

    if (level == DEBUG) {
      Serial.print(F("DEBUG"));

    } else if (level == INFO) {
      Serial.print(F("INFO"));

    } else if (level == WARNING) {
      Serial.print(F("WARNING"));

    } else if (level == ERROR) {
      Serial.print(F("ERROR"));

    } else if (level == CRITICAL) {
      Serial.print(F("CRITICAL"));
    }

    Serial.println();
  };

  /**
   * @brief Overloads the () operator to log messages.
   * Example usage:
   *
   * ramen::Logger Log;
   * Log.setLogLevel(ramen::ERROR);
   * Log(ramen::ERROR, "This is the error message %u \n", 1);
   *
   * @param givenLevel Logging level
   * @param format The message itself
   */
  void operator()(LogLevel givenLevel, const char* format...) {
    va_list args;
    va_start(args, format);

    vsnprintf(str, 200, format, args);

    // Print the message if the logging level is equal or higher than the set
    // logging level
    if (givenLevel >= level) {
      // Print the correct message label
      switch (givenLevel) {
        case DEBUG:
          Serial.print(F("[DEBUG]"));
          break;
        case INFO:
          Serial.print(F("[INFO]"));
          break;
        case WARNING:
          Serial.print(F("[WARNING]"));
          break;
        case ERROR:
          Serial.print(F("[ERROR]"));
          break;
        case CRITICAL:
          Serial.print(F("[CRITICAL]"));
          break;
      }

      // Print the message itself
      Serial.print(F(" "));
      Serial.print(str);
    }

    va_end(args);
  }
};

}  // namespace ramen

#endif
