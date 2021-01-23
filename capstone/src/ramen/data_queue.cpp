#include "ramen/data_queue.hpp"

#include <Arduino.h>

ramen::DataQueue::DataQueue(){};

void ramen::DataQueue::pop(uint32_t address){};
void ramen::DataQueue::push(uint32_t address, std::string data){};