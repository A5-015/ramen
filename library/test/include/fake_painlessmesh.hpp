/**
 * Used to provide the functionality of painlessMesh without actually importing
 * it
 *
 */

#ifndef _RAMEN_FAKE_PAINLESSMESH_HPP_
#define _RAMEN_FAKE_PAINLESSMESH_HPP_

class painlessMesh {
 private:
  uint32_t node_id = 1;
  uint32_t node_time = 0;

 public:
  void init(std::string mesh_name, std::string mesh_password,
            Scheduler* scheduler, long mesh_port){};

  uint32_t getNodeId() { return node_id; };

  uint32_t getNodeTime() { return node_time; };
};

#endif