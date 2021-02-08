/**
 * Used to provide the functionality of painlessMesh without actually importing
 * it
 *
 */

#ifndef _RAMEN_FAKE_PAINLESSMESH_HPP_
#define _RAMEN_FAKE_PAINLESSMESH_HPP_

#include <list>
#include <vector>

#include "catch_common.hpp"

namespace fake_painlessmesh {

typedef std::function<void(uint32_t from, string_t& msg)> receivedCallback_t;

struct Node {
  uint32_t node_id;
  uint32_t node_time;
};

class painlessMesh {
 private:
  std::string _mesh_name;
  std::string _mesh_password;
  long _mesh_port;
  std::list<fake_painlessmesh::Node> _nodes;
  std::list<uint32_t> _node_list;
  uint32_t _mesh_time = 0;

 public:
  ///////////////////////////////////////////////////
  // Methods for faking painlessMesh functionality //
  ///////////////////////////////////////////////////
  void init(std::string mesh_name,
            std::string mesh_password,
            Scheduler* scheduler_ptr,
            long mesh_port) {
    this->_mesh_name = mesh_name;
    this->_mesh_password = mesh_password;
    this->_mesh_port = mesh_port;
  };

  uint32_t getNodeId() {
    return 0;
  };

  uint32_t getNodeTime() {
    return this->_mesh_time;
  };

  void update() {

  };

  bool sendBroadcast(string_t data) {
    return true;
  };

  std::list<uint32_t> getNodeList(bool include_self = false) {
    return _node_list;
  };

  void onReceive(receivedCallback_t on_receive) {

  };

  bool sendSingle(uint32_t destination_id, string_t data) {
    return true;
  };

  /////////////////////////////////////////////////////
  // Methods used for modifying the mesh for testing //
  /////////////////////////////////////////////////////
  void addNode() {};

  void setMeshTime(uint32_t time) {
    this->_mesh_time = time;
  };

  uint32_t getMeshTime() {
    return this->_mesh_time;
  };
};

} // namespace fake_painlessmesh

#endif