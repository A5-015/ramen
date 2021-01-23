#include "ramen/log_holder.hpp"

#include <Arduino.h>

ramen::LogHolder::LogHolder(){};

uint32_t ramen::LogHolder::getMatchIndex(uint32_t address) {
  uint32_t index;
  return index;
};
void ramen::LogHolder::setMatchIndex(uint32_t address, uint32_t index){};

uint32_t ramen::LogHolder::getNextIndex(uint32_t address) {
  uint32_t index;
  return index;
};
void ramen::LogHolder::setNextIndex(uint32_t address, uint32_t index){};

void ramen::LogHolder::advanceCommitIndex(uint32_t address){};