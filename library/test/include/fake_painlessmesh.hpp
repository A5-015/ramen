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

struct node {
  uint32_t node_id;
  uint32_t node_time;
};

class painlessMesh {
 private:
  std::string mesh_name;
  std::string mesh_password;
  long mesh_port;
  std::list<fake_painlessmesh::node> nodes;
  uint32_t mesh_time = 0;

 public:
  ///////////////////////////////////////////////////
  // Methods for faking painlessMesh functionality //
  ///////////////////////////////////////////////////
  void init(std::string mesh_name, std::string mesh_password,
            Scheduler* scheduler, long mesh_port) {
    this->mesh_name = mesh_name;
    this->mesh_password = mesh_password;
    this->mesh_port = mesh_port;
  };

  uint32_t getNodeId() { return 0; };

  uint32_t getNodeTime() { return mesh_time; };

  /////////////////////////////////////////////////////
  // Methods used for modifying the mesh for testing //
  /////////////////////////////////////////////////////
  void addNode(){};

  void setMeshTime(uint32_t time) { this->mesh_time = time; };

  uint32_t getMeshTime() { return mesh_time; };
};

}  // namespace fake_painlessmesh

#endif