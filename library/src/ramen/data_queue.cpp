/**
 * @file data_queue.cpp
 * @brief data_queue.cpp
 *
 */
#include "ramen/data_queue.hpp"

using namespace broth::dataqueue;

DataQueue::DataQueue() {};

void DataQueue::pop() {
  return this->_entries.pop();
};

void DataQueue::push(string_t data) {
  this->_entries.push(data);
};

bool DataQueue::checkEmpty() {
  return this->_entries.empty();
};