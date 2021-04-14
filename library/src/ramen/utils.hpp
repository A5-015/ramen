/**
 * @file utils.hpp
 * @brief utils.hpp
 *
 */
#ifndef _RAMEN_UTILS_HPP_
#define _RAMEN_UTILS_HPP_

#include "ramen/configuration.hpp"
#include "ramen/logger.hpp"

namespace broth {
namespace utils {
  using namespace broth::logger;

  /**
   * @brief A class for setting timers and checking if specified period elapsed.
   * This class is not a scheduler, the check() member function needs to be
   * called in an loop.
   *
   */
  class Timer {
   private:
    uint32_t _previous_time;
    uint32_t _period;

   public:
    /**
     * @brief Construct a new Timer object
     *
     */
    Timer();

    /**
     * @brief Initialize the timer with the given period in microseconds
     *
     * @param period
     */
    void init(uint32_t period);

    /**
     * @brief Used to check if the set period elapsed or not
     *
     * @param current_time The current time in the mesh during the check
     * @return true If the set period elapsed
     * @return false If the set period did not elapse
     */
    bool check(uint32_t current_time);
  };
} // namespace utils
} // namespace broth

#endif