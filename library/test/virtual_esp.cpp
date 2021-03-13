// #include <thread>
// std::thread t1(&Server::update, &ramen_1);
// t1.join();

#include <cxxopts.hpp>
#include <vector>

#include "ramen.h"

using namespace broth::logger;
using namespace broth::server;
using namespace broth::meshnetwork;

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

  std::cout << "\n\033[95m>> Calling broth::server::Server::init() for all "
               "nodes:\033[0m\n";

  // Generate the nodes
  for(uint32_t i = 0; i < result["nodes"].as<int>(); ++i) {
    // Generate node
    nodes.push_back(new Server());

    // Set node ID, start IDs from 1
    nodes.back()->_mesh.setNodeId(i + 1);

    // Initialize the node
    nodes.back()->init(MESH_NAME,
                       MESH_PASSWORD,
                       MESH_PORT,
                       broth::logger::DEBUG);

    // // Override node's internal logging levels
    // nodes.back()->_logger.setLogLevel(broth::logger::DEBUG);
  }

  // Create the connections between nodes wihtin the virtual mesh network
  for(uint32_t i = 0; i < result["nodes"].as<int>(); ++i) {
    for(uint32_t j = 0; j < result["nodes"].as<int>(); ++j) {
      if(nodes[i]->_mesh._node_id != nodes[j]->_mesh._node_id) {
        // Add neighbour nodes except the node itself
        nodes[i]->_mesh.addNeighbourNode(nodes[j]->_mesh);
      }
    }
  }

  std::cout << "\n\033[95m>> Calling broth::server::Server::update() in a "
               "loop for all nodes:\033[0m\n";

  uint32_t iteration = 0;
  bool printed_output = false;

  // Simulates loop() from Arduino
  while(true) {
    for(uint32_t j = 0; j < result["nodes"].as<int>(); ++j) {
      nodes[j]->update();

      // Check if anything was outputted
      printed_output = (printed_output || nodes[j]->_logger._printed_output);

      // Reset the flag for the specific node
      nodes[j]->_logger._printed_output = false;
    }

    // Print the iteration number if anything was outputted to the terminal
    if((printed_output) && (iteration > 0)) {
      std::cout << "\033[95m^^ loop() iteration #" << iteration
                << " @ mesh time " << nodes.back()->_mesh.getMeshTime()
                << "\033[0m\n\n";
    }

    if(nodes.back()->_mesh.getMeshTime() >
       (result["time"].as<float>() * 1000000)) {
      break;
    }

    printed_output = false;
    iteration++;
  }

  return 0;
}