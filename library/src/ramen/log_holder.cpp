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

void LogHolder::resetMatchIndexMap(std::list<uint32_t> *node_list_ptr) {
  this->_match_index_ptr = new std::unordered_map<uint32_t, uint32_t>;
  for(auto it = node_list_ptr->begin(); it != node_list_ptr->end(); ++it) {
    this->_match_index_ptr->insert(std::make_pair(*it, 0));
  }
};

void LogHolder::resetNextIndexMap(std::list<uint32_t> *node_list_ptr) {
  this->_next_index_ptr = new std::unordered_map<uint32_t, uint32_t>;
  for(auto it = node_list_ptr->begin(); it != node_list_ptr->end(); ++it) {
    this->_next_index_ptr->insert(std::make_pair(*it, 1));
  }
};

uint32_t LogHolder::getLogSize() {
  return _entries.size();
}

uint32_t LogHolder::getLastLogTerm() {
  return _entries.size() < 1 ? 0 : _entries.back().first;
}
