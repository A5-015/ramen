/**
 * @file log_holder.hpp
 * @brief log_holder.hpp
 *
 */
#ifndef _RAMEN_LOG_HOLDER_HPP_
#define _RAMEN_LOG_HOLDER_HPP_

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "ramen/configuration.hpp"

namespace broth {
namespace logholder {

  /**
   * @brief Holds the logs of the consensus for the given server
   *
   */
  class LogHolder {
   private:
    // entries:{term, data}
    std::vector<std::pair<uint32_t, string_t>> _entries;

    // match_index_ptr:{server_id, index_of_last_recvd_log_entry_by_server_id}
    std::unordered_map<uint32_t, uint32_t> *_match_index_ptr = NULL;

    // next_index_ptr:{server_id, index_of_next_log_entry_to_send_to_server}
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
     * @brief Given the server ID, returns the index of next log entry to send
     * from _next_index_ptr
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
    void resetMatchIndexMap(std::list<uint32_t> *node_list_ptr, uint32_t index);

    /**
     * @brief  Set the next index for all nodes to 1
     *
     * @param nodeList Node list obtained from painlessMesh
     */
    void resetNextIndexMap(std::list<uint32_t> *node_list_ptr, uint32_t index);

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

    /**
     * @brief Get the term in entries vector given the index
     *
     * @param log_index
     * @return uint32_t
     */
    uint32_t getLogTerm(uint32_t log_index);

    /**
     * @brief Extract the index of last received log entries by all servers from
     * match_index_ptr and return the lowest index the majority of servers have
     * commmitted
     *
     * From the Raft paper:
     * "A log entry is committed once the leader that created the entry has
     * replicated it on a majority of the servers"
     *
     * @return uint32_t
     */
    uint32_t getMajorityCommitIndex();
  };

} // namespace logholder
} // namespace broth

#endif