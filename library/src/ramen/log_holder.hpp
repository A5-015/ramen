#ifndef _RAMEN_LOG_HOLDER_HPP_
#define _RAMEN_LOG_HOLDER_HPP_

#include <string>
#include <unordered_map>
#include <vector>

#include "ramen/configuration.hpp"

namespace ramen {
namespace logholder {

/**
 * @brief Holds the logs of the consensus for the given server
 *
 */
class LogHolder {
 private:
  std::vector<String> entries;

  std::unordered_map<uint32_t, uint32_t> matchIndex;
  std::unordered_map<uint32_t, uint32_t> nextIndex;

 public:
  /**
   * @brief Construct a new Log Holder object
   *
   */
  LogHolder();

  /**
   * @brief Get the Match Index object
   *
   * @param address
   * @return uint32_t
   */
  uint32_t getMatchIndex(uint32_t address);

  /**
   * @brief Set the Match Index object
   *
   * @param address
   * @param index
   */
  void setMatchIndex(uint32_t address, uint32_t index);

  /**
   * @brief Get the Next Index object
   *
   * @param address
   * @return uint32_t
   */
  uint32_t getNextIndex(uint32_t address);

  /**
   * @brief Set the Next Index object
   *
   * @param address
   * @param index
   */
  void setNextIndex(uint32_t address, uint32_t index);

  /**
   * @brief
   *
   * @param address
   */
  void advanceCommitIndex(uint32_t address);
};

}  // namespace logholder
}  // namespace ramen

#endif