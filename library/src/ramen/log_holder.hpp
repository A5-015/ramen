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
    std::vector<std::pair<uint32_t, string_t>> _entries;

    std::unordered_map<uint32_t, uint32_t> *_match_index_ptr = NULL;
    std::unordered_map<uint32_t, uint32_t> *_next_index_ptr = NULL;

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

    /**
     * @brief  Set the match index for all nodes to 0
     *
     * @param nodeList Node list obtained from painlessMesh
     */
    void resetMatchIndexMap(std::list<uint32_t> *node_list_ptr);

    /**
     * @brief  Set the next index for all nodes to 1
     *
     * @param nodeList Node list obtained from painlessMesh
     */
    void resetNextIndexMap(std::list<uint32_t> *node_list_ptr);

    /**
     * @brief Get the size of the entries
     *
     * @return uint32_t
     */
    uint32_t getLogSize();

    /**
     * @brief Get the last log entry's term
     *
     * @return uint32_t
     */
    uint32_t getLastLogTerm();
  };

} // namespace logholder
} // namespace ramen

#endif