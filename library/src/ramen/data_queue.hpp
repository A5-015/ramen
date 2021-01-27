#ifndef _RAMEN_DATA_QUEUE_HPP_
#define _RAMEN_DATA_QUEUE_HPP_

#include <queue>
#include <tuple>

#include "ramen/configuration.hpp"

namespace ramen {
namespace dataqueue {

/**
 * @brief Stores the data received from a client until it is moved to the log
 * holder
 *
 */
class DataQueue {
 private:
  std::queue<std::tuple<uint32_t, string_t>> entries;

 public:
  /**
   * @brief Construct a new Data Queue object
   *
   */
  DataQueue();

  /**
   * @brief
   *
   * @param address
   */
  void pop(uint32_t address);

  /**
   * @brief
   *
   * @param address
   * @param data
   */
  void push(uint32_t address, string_t data);
};

}  // namespace dataqueue
}  // namespace ramen

#endif