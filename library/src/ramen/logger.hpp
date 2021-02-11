/**
 * @file logger.hpp
 * @brief logger.hpp
 *
 */
#ifndef _RAMEN_LOGGER_HPP_
#define _RAMEN_LOGGER_HPP_

#include <stdarg.h>

#include "ramen/configuration.hpp"

namespace broth {
namespace logger {

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
    uint8_t _level = 0;
    uint32_t id;
    char _str[200];

   public:
    /**
     * @brief Set the id for the logger
     *
     * @param id
     */
    void setLoggerId(uint32_t id) {
      this->id = id;
    }

    /**
     * @brief Set the logging level
     *
     * @param givenLevel DEBUG or INFO or WARNING or ERROR or CRITICAL
     */
    void setLogLevel(uint8_t given_level) {
      _level = given_level;

// clang-format off
      // Print the id of the node while testing
      #ifdef _RAMEN_CATCH_TESTING_
      string_t s = std::to_string(this->id);
      Serial.print(F("[ID:"));
      Serial.print(s.c_str());
      Serial.print(F("] "));
      #endif
      // clang-format on

      Serial.print(F("Logging level is set to: "));

      if(_level == DEBUG) {
        Serial.print(F("DEBUG"));

      } else if(_level == INFO) {
        Serial.print(F("INFO"));

      } else if(_level == WARNING) {
        Serial.print(F("WARNING"));

      } else if(_level == ERROR) {
        Serial.print(F("ERROR"));

      } else if(_level == CRITICAL) {
        Serial.print(F("CRITICAL"));
      }

      Serial.println();
    };

    /**
     * @brief Overloads the () operator to log messages.
     * Example usage:
     *
     * broth::Logger Log;
     * Log.setLogLevel(broth::ERROR);
     * Log(broth::ERROR, "This is the error message %u \n", 1);
     *
     * @param givenLevel Logging level
     * @param format The message itself
     */
    void operator()(LogLevel given_level, const char* format_ptr...) {
      va_list args;
      va_start(args, format_ptr);

      vsnprintf(_str, 200, format_ptr, args);

      // Print the message if the logging level is equal or higher than the
      // set logging level
      if(given_level >= _level) {
// clang-format off
        // Print the id of the node while testing
        #ifdef _RAMEN_CATCH_TESTING_
        string_t s = std::to_string(this->id);
        Serial.print(F("[ID:"));
        Serial.print(s.c_str());
        Serial.print(F("]"));
        #endif
        // clang-format on

        // Print the correct message label
        switch(given_level) {
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
        Serial.print(_str);
      }

      va_end(args);
    }

    void operator()(LogLevel given_level, string_t& message) {
      // Print the message if the logging level is equal or higher than the
      // set logging level
      if(given_level >= _level) {
// clang-format off
        // Print the id of the node while testing
        #ifdef _RAMEN_CATCH_TESTING_
        string_t s = std::to_string(this->id);
        Serial.print(F("[ID:"));
        Serial.print(s.c_str());
        Serial.print(F("]"));
        #endif
        // clang-format on

        // Print the correct message label
        switch(given_level) {
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
        Serial.print(message);
      }
    }
  };

} // namespace logger
} // namespace broth

#endif
