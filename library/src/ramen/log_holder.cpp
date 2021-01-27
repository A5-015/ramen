#include "ramen/log_holder.hpp"

using namespace ramen::logholder;

LogHolder::LogHolder(){};

uint32_t LogHolder::getMatchIndex(uint32_t address) {
  uint32_t index = 0;
  return index;
};
void LogHolder::setMatchIndex(uint32_t address, uint32_t index){};

uint32_t LogHolder::getNextIndex(uint32_t address) {
  uint32_t index = 0;
  return index;
};
void LogHolder::setNextIndex(uint32_t address, uint32_t index){};

void LogHolder::advanceCommitIndex(uint32_t address){};