/**
 * @file log_holder.cpp
 * @brief log_holder.cpp
 *
 */
#include "ramen/log_holder.hpp"

using namespace broth::logholder;

LogHolder::LogHolder() {};

uint32_t LogHolder::getMatchIndex(uint32_t address) {
  return (*(this->_match_index_ptr))[address];
};

void LogHolder::setMatchIndex(uint32_t address, uint32_t index) {};

uint32_t LogHolder::getNextIndex(uint32_t address) {
  return (*(this->_next_index_ptr))[address];
};

void LogHolder::setNextIndex(uint32_t address, uint32_t index) {};

void LogHolder::advanceCommitIndex(uint32_t address) {};

void LogHolder::resetMatchIndexMap(std::list<uint32_t> *node_list_ptr,
                                   uint32_t index) {
  delete this->_match_index_ptr;
  this->_match_index_ptr = new std::unordered_map<uint32_t, uint32_t>;
  for(auto it = node_list_ptr->begin(); it != node_list_ptr->end(); ++it) {
    this->_match_index_ptr->insert(std::make_pair(*it, index));
  }
};

void LogHolder::resetNextIndexMap(std::list<uint32_t> *node_list_ptr,
                                  uint32_t index) {
  delete this->_next_index_ptr;
  this->_next_index_ptr = new std::unordered_map<uint32_t, uint32_t>;
  for(auto it = node_list_ptr->begin(); it != node_list_ptr->end(); ++it) {
    this->_next_index_ptr->insert(std::make_pair(*it, index));
  }
};

uint32_t LogHolder::getLogSize() {
  return _entries.size();
}

uint32_t LogHolder::getLastLogTerm() {
  return _entries.size() < 1 ? 0 : _entries.back().first;
}

uint32_t LogHolder::getLogTerm(uint32_t log_index) {
  if(log_index < 1 || log_index > this->_entries.size()) {
    return 0;
  } else {
    return _entries[log_index].first;
  }
}

uint32_t LogHolder::getMajorityCommitIndex() {
  std::vector<uint32_t> match_indices(_match_index_ptr->size());
  uint32_t i = 0;

  // Extract match indices from match_index_ptr
  for(auto m : *(_match_index_ptr)) {
    match_indices[i] = m.second;
    i++;
  }

  // Sort vector
  std::sort(match_indices.begin(), match_indices.end());

  // Return majority
  return match_indices[match_indices.size() / 2];
}

void LogHolder::popEntry() {
  this->_entries.pop_back();
}

void LogHolder::pushEntry(std::pair<uint32_t, string_t> new_entry) {
  this->_entries.push_back(new_entry);
}