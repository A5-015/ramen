/**
 * @file data_queue.cpp
 * @brief data_queue.cpp
 *
 */
#include "ramen/data_queue.hpp"

using namespace broth::dataqueue;

DataQueue::DataQueue() {};

string_t DataQueue::pop() {
  string_t data = this->_entries.front();
  this->_entries.pop();
  return data;
};

void DataQueue::push(string_t data) {
  this->_entries.push(data);
};

bool DataQueue::checkEmpty() {
  return this->_entries.empty();
};