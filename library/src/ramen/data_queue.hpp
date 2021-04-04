/**
 * @file data_queue.hpp
 * @brief data_queue.hpp
 *
 */
#ifndef _RAMEN_DATA_QUEUE_HPP_
#define _RAMEN_DATA_QUEUE_HPP_

#include <queue>
#include <tuple>

#include "ramen/configuration.hpp"

namespace broth {
namespace dataqueue {

  /**
   * @brief Stores the data received from a client until it is moved to the
   * consensus leader's log holder
   *
   */
  class DataQueue {
   private:
    std::queue<string_t> _entries;

   public:
    /**
     * @brief Construct a new Data Queue object
     *
     */
    DataQueue();

    /**
     * @brief Pop the oldest data in the queue to send to consensus leader
     *
     */
    void pop();

    /**
     * @brief Push new data in the queue to eventually send to consensus leader
     *
     * @param data
     */
    void push(string_t data);

    /**
     * @brief Checks if there is data in the queue
     *
     * @return true
     * @return false
     */
    bool checkEmpty();
  };

} // namespace dataqueue
} // namespace broth

#endif