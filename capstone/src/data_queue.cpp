#include "ramen/data_queue.hpp"

#include <Arduino.h>

uint32_t ramen::DataQueue::getMatchIndex(uint32_t address){};
void ramen::DataQueue::setMatchIndex(uint32_t address, uint32_t index){};

uint32_t ramen::DataQueue::getNextIndex(uint32_t address){};
void ramen::DataQueue::setNextIndex(uint32_t address, uint32_t index){};

void ramen::DataQueue::advanceCommitIndex(uint32_t address){};