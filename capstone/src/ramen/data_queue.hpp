#ifndef _RAMEN_DATA_QUEUE_HPP_
#define _RAMEN_DATA_QUEUE_HPP_

#include <string>
#include <unordered_map>

#include "ramen/configuration.hpp"

namespace ramen {

/**
 * @brief Stores the data received from a client until it is moved to the log
 * holder
 *
 */
namespace DataQueue {
class DataQueue {
 private:
  std::vector<std::string> entries;

  std::unordered_map<uint32_t, uint32_t> matchIndex;
  std::unordered_map<uint32_t, uint32_t> nextIndex;

 public:
  uint32_t getMatchIndex(uint32_t address);
  void setMatchIndex(uint32_t address, uint32_t index);

  uint32_t getNextIndex(uint32_t address);
  void setNextIndex(uint32_t address, uint32_t index);

  void advanceCommitIndex(uint32_t address);
};

}  // namespace DataQueue
}  // namespace ramen

#endif