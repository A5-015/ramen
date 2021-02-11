// #include <thread>
// std::thread t1(&Server::update, &ramen_1);
// t1.join();

#include <cxxopts.hpp>
#include <vector>

#include "ramen.h"

using namespace broth::logger;
using namespace broth::server;

int main(int argc, char** argv) {
  cxxopts::Options options("ramen virtual test network",
                           "Virtual ESP8266 network testing for ramen");

  // clang-format off
  options.add_options()
    ("t,time", "simulation duration", cxxopts::value<float>()->default_value("0.5"))
    ("n,nodes", "number of nodes", cxxopts::value<int>()->default_value("3"))
    ;
  // clang-format on

  auto result = options.parse(argc, argv);

  std::vector<Server*> nodes;

  // Generate the nodes
  for(uint8_t i = 0; i < result["nodes"].as<int>(); ++i) {
    // Generate node
    nodes.push_back(new Server());
    // Set node ID
    nodes.back()->_mesh.setNodeId(i);
    // Initialize the node
    nodes.back()->init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
    // Override node's internal logging levels
    nodes.back()->_logger.setLogLevel(broth::logger::DEBUG);
  }

  // Create the connections between nodes
  for(uint8_t i = 0; i < result["nodes"].as<int>(); ++i) {
    for(uint8_t j = 0; j < result["nodes"].as<int>(); ++j) {
      if(nodes[i]->_mesh._node_id != nodes[j]->_mesh._node_id) {
        // Add neighbour nodes except the node itself
        nodes[i]->_mesh.addNeighbourNode(nodes[j]->_mesh);
      }
    }
  }

  // Simulates loop() from Arduino
  while(true) {
    for(uint8_t j = 0; j < result["nodes"].as<int>(); ++j) {
      nodes[j]->update();
    }
    if(nodes.back()->_mesh.getMeshTime() >
       (result["time"].as<float>() * 1000000)) {
      break;
    }
  }

  return 0;
}