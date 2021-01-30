/**
 * Used to provide the functionality of painlessMesh without actually importing
 * it
 *
 */

#ifndef _RAMEN_FAKE_PAINLESSMESH_HPP_
#define _RAMEN_FAKE_PAINLESSMESH_HPP_

#include <list>
#include <vector>

namespace fake_painlessmesh {

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