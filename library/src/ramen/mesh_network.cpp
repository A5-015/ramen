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

      this->_node_id = this->_painless_mesh.getNodeId();

      this->_logger(DEBUG, "Just initialized painlessMesh!\n");
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING,
                    "(init) Selected mesh network type does not exist!\n");
      return false;
      break;
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
      this->_logger(WARNING,
                    "(update) Selected mesh network type does not exist!\n");
      return false;
      break;
  }

  return true;
};

uint32_t _meshnetwork::getNodeId() {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.getNodeId();
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING,
                    "(getNodeId) Selected mesh network type does not exist!\n");
      return 0;
      break;
  }
};

uint32_t _meshnetwork::getNodeTime() {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.getNodeTime();
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(getNodeTime) Selected mesh network type does not exist!\n");
      return 0;
      break;
  }
};

uint32_t _meshnetwork::getMeshTime() {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.getNodeTime();
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(getMeshTime) Selected mesh network type does not exist!\n");
      return 0;
      break;
  }
};

std::list<uint32_t> _meshnetwork::getNodeList(bool include_self) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.getNodeList(include_self);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(getNodeList) Selected mesh network type does not exist!\n");
      return std::list<uint32_t>();
      break;
  }
};

bool _meshnetwork::sendBroadcast(string_t data) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.sendBroadcast(data);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(sendBroadcast) Selected mesh network type does not exist!\n");
      return false;
      break;
  }
};

bool _meshnetwork::sendMessageToNode(uint32_t destination_node_id,
                                     string_t data) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      return this->_painless_mesh.sendSingle(destination_node_id, data);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(sendMessageToNode) Selected mesh network type does not exist!\n");
      return false;
      break;
  }
};

void _meshnetwork::onReceiveCallback(received_callback_t on_receive) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.onReceive(on_receive);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(WARNING,
                    "(onReceive) Selected mesh network type does not exist!\n");
      break;
  }
};

  /////////////////////////////////////////////////
  // Methods used only during testing
  /////////////////////////////////////////////////

#ifdef _RAMEN_UNIT_TESTING_

void _meshnetwork::setMeshTime(uint32_t time) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.setMeshTime(time);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(addNeighbourNode) Selected mesh network type does not exist!\n");
      break;
  }
};

void _meshnetwork::incrementMeshTimeBy(uint32_t time) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.incrementMeshTimeBy(time);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(addNeighbourNode) Selected mesh network type does not exist!\n");
      break;
  }
};

void _meshnetwork::setNodeId(uint32_t node_id) {
  this->_node_id = node_id;

  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.setNodeId(node_id);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(addNeighbourNode) Selected mesh network type does not exist!\n");
      break;
  }
};

void _meshnetwork::addNeighbourNode(MeshNetwork& neighbour_node) {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.addNeighbourNode(neighbour_node._painless_mesh);
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(addNeighbourNode) Selected mesh network type does not exist!\n");
      break;
  }
};

void _meshnetwork::checkForNewMessages() {
  switch(this->_selected_mesh_network_type) {
    case PAINLESSMESH:
      this->_painless_mesh.checkForNewMessages();
      break;

    default:
      // A wrong mesh type was specified
      this->_logger(
          WARNING,
          "(addNeighbourNode) Selected mesh network type does not exist!\n");
      break;
  }
};

#endif