#ifndef _RAMEN_LOG_HOLDER_HPP_
#define _RAMEN_LOG_HOLDER_HPP_

#include <queue>
#include <string>
#include <tuple>

#include "ramen/configuration.hpp"

namespace ramen {

/**
 * @brief Holds the logs of the consensus for the given server
 *
 */
class LogHolder {
 private:
  std::queue<std::tuple<uint32_t, std::string>> entries;

 public:
  void pop(uint32_t address);
  void push(uint32_t address, std::string data);
};

}  // namespace ramen

#endif