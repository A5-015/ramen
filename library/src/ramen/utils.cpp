/**
 * @file utils.cpp
 * @brief utils.cpp
 *
 */
#include "ramen/utils.hpp"

using _timer = broth::utils::Timer;
using namespace broth::logger;

_timer::Timer() {};

void _timer::init(uint32_t period) {
  this->_period = period;
  this->_previous_time = 0;
};

bool _timer::check(uint32_t current_time) {
  if((current_time - _previous_time) > _period) {
    _previous_time = current_time;
    return true;
  } else {
    return false;
  }
};