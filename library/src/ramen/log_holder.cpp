#include "ramen/log_holder.hpp"

using namespace ramen::logholder;

LogHolder::LogHolder() {};

uint32_t LogHolder::getMatchIndex(uint32_t address) {
  uint32_t index = 0;
  return index;
};

void LogHolder::setMatchIndex(uint32_t address, uint32_t index) {};

uint32_t LogHolder::getNextIndex(uint32_t address) {
  uint32_t index = 0;
  return index;
};

void LogHolder::setNextIndex(uint32_t address, uint32_t index) {};

void LogHolder::advanceCommitIndex(uint32_t address) {};

void LogHolder::resetMatchIndexMap(std::list<uint32_t> *nodeList) {
  this->matchIndex_ptr = new std::unordered_map<uint32_t, uint32_t>;
  for(auto it = nodeList->begin(); it != nodeList->end(); ++it) {
    this->matchIndex_ptr->insert(std::make_pair(*it, 0));
  }
};

void LogHolder::resetNextIndexMap(std::list<uint32_t> *nodeList) {
  this->nextIndex_ptr = new std::unordered_map<uint32_t, uint32_t>;
  for(auto it = nodeList->begin(); it != nodeList->end(); ++it) {
    this->nextIndex_ptr->insert(std::make_pair(*it, 1));
  }
};

uint32_t LogHolder::getLogSize() {
  return entries.size();
}

uint32_t LogHolder::getLastLogTerm() {
  return entries.size() < 1 ? 0 : entries.back().first;
}
