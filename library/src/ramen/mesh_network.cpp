/**
 * @file mesh_network.cpp
 * @brief mesh_network.cpp
 *
 */

#include "ramen/mesh_network.hpp"

using _meshnetwork = broth::meshnetwork::MeshNetwork;
using namespace broth::logger;

_meshnetwork::MeshNetwork() {

};

bool _meshnetwork::init(MeshNetworkType mesh_network_type,
                        string_t mesh_name,
                        string_t mesh_password,
                        uint16_t mesh_port,
                        uint8_t logging_level) {
  this->_logger.setLogLevel(logging_level);

  // Set mesh network type
  this->_selected_mesh_network_type = mesh_network_type;

  // Initialize selected mesh network
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.init(mesh_name,
                                mesh_password,
                                &_scheduler,
                                mesh_port);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
      return false;
  }

  return true;
};

bool _meshnetwork::update() {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.update();
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
      return false;
  }

  return true;
};

uint32_t _meshnetwork::getNodeId() {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.getNodeId();

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
      return 0;
  }
};

uint32_t _meshnetwork::getNodeTime() {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.getNodeTime();

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
      return 0;
  }
};

std::list<uint32_t> _meshnetwork::getNodeList(bool include_self) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.getNodeList(include_self);

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
      return std::list<uint32_t>();
  }
};

bool _meshnetwork::sendBroadcast(string_t data) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.sendBroadcast(data);

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
      return false;
  }
};

bool _meshnetwork::sendMessageToNode(uint32_t destination_node_id,
                                     string_t data) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.sendSingle(destination_node_id, data);

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
      return false;
  }
};

void _meshnetwork::onReceive(received_callback_t on_receive) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.onReceive(on_receive);
    default:
      // A wrong mesh type was specified
      this->_logger(WARNING, "Selected mesh network type does not exist!\n");
  }
};