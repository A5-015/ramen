#include "ramen/data_queue.hpp"

#include <Arduino.h>

using namespace ramen::dataqueue;

DataQueue::DataQueue(){};

void DataQueue::pop(uint32_t address){};
void DataQueue::push(uint32_t address, String data){};