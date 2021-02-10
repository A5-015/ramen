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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

namespace fake_painlessmesh {

typedef std::function<void(uint32_t from, string_t& msg)> receivedCallback_t;

struct Node {
  uint32_t node_id;
  std::list<std::pair<uint32_t, string_t>>* message_buffer_ptr;
};

class painlessMesh {
  // private:
 public:
  std::string _mesh_name;
  std::string _mesh_password;
  long _mesh_port;
  std::list<fake_painlessmesh::Node> _nodes;
  std::list<std::pair<uint32_t, string_t>> _message_buffer;
  std::list<uint32_t> _node_list;
  uint32_t _mesh_time = 0;
  uint32_t _node_id;
  receivedCallback_t _received_callback;

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
    if(scheduler_ptr) {
      // Needed to silence -Werror=unused-parameter here only
    }
  };

  uint32_t getNodeId() {
    return this->_node_id;
  };

  uint32_t getNodeTime() {
    return this->_mesh_time;
  };

  void update() {
    // Increment the mesh time
    this->incrementMeshTimeBy(10000);
    // Check if there is a new message and notify the node via onReceive
    // callback
    this->checkForNewMessages();
  };

  bool sendBroadcast(string_t data, bool include_self = false) {
    if(include_self) {
      printf(ANSI_COLOR_GREEN
             "[ID:%u @ %u] sendBroadcast was called with include "
             "self\n" ANSI_COLOR_RESET,
             this->_node_id,
             this->_mesh_time);
    }

    for(auto const& node : this->_nodes) {
      this->sendSingle(node.node_id, data);
    }

    return true;
  };

  std::list<uint32_t> getNodeList(bool include_self = false) {
    if(include_self) {
      printf(ANSI_COLOR_GREEN
             "[ID:%u @ %u] getNodeList was called with include "
             "self\n" ANSI_COLOR_RESET,
             this->_node_id,
             this->_mesh_time);
    }

    return this->_node_list;
  };

  void onReceive(receivedCallback_t on_receive) {
    this->_received_callback = on_receive;
  };

  bool sendSingle(uint32_t destination_id, string_t data) {
    // Find the pointer to the message buffer of the destination node
    for(auto const& node : this->_nodes) {
      if(node.node_id == destination_id) {
        // Insert the message into the target node
        // Here we use this node's id instead of the destination id because,
        // it will serve as the from address on the destination node
        node.message_buffer_ptr->push_back(
            std::make_pair(this->_node_id, data));
        printf(ANSI_COLOR_GREEN
               "[ID:%u @ %u] Sent message to node %u: %s\n" ANSI_COLOR_RESET,
               this->_node_id,
               this->_mesh_time,
               destination_id,
               data.c_str());
        return true;
      }
    }

    // Return false if the destination node was not found
    printf(ANSI_COLOR_RED
           "[ID:%u @ %u] Requested destination id (%u) is not "
           "found\n" ANSI_COLOR_RESET,
           this->_node_id,
           this->_mesh_time,
           destination_id);
    return false;
  };

  /////////////////////////////////////////////////////
  // Methods used for modifying the mesh for testing //
  /////////////////////////////////////////////////////
  void setMeshTime(uint32_t time) {
    this->_mesh_time = time;
  };

  void incrementMeshTimeBy(uint32_t time) {
    this->_mesh_time += time;
  };

  uint32_t getMeshTime() {
    return this->_mesh_time;
  };

  void setNodeId(uint32_t node_id) {
    this->_node_id = node_id;
  }

  void addNeighbourNode(painlessMesh& neighbour_node) {
    Node node = {};
    node.node_id = neighbour_node._node_id;
    node.message_buffer_ptr = &neighbour_node._message_buffer;

    this->_nodes.push_back(node);
    this->_node_list.push_back(neighbour_node._node_id);
  };

  void checkForNewMessages() {
    // Process all new messages
    while(this->_message_buffer.size() > 0) {
      // Read the message
      auto message = this->_message_buffer.front();

      printf(ANSI_COLOR_CYAN
             "[ID:%u @ %u] New message from %u: %s\n" ANSI_COLOR_RESET,
             this->_node_id,
             this->_mesh_time,
             message.first,
             message.second.c_str());

      // Run the callback
      this->_received_callback(message.first, message.second);

      // Remove the message
      this->_message_buffer.pop_front();
    }
  };
};

} // namespace fake_painlessmesh

#endif