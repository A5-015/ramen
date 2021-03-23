#include <cxxopts.hpp>
#include <thread>
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
    ("t,time", "simulation duration", cxxopts::value<float>()->default_value("10"))
    ("n,nodes", "number of nodes", cxxopts::value<int>()->default_value("3"))
    ;
  // clang-format on

  auto result = options.parse(argc, argv);

  uint32_t target_number_of_nodes = result["nodes"].as<int>();

  std::vector<Server*> nodes;

  std::cout << "\n\033[95m>> Calling broth::server::Server::init() for all "
               "nodes:\033[0m\n";

  ////////////////////////////////////
  // Simulates setup() from Arduino //
  ////////////////////////////////////

  // Generate the nodes
  for(uint32_t i = 0; i < target_number_of_nodes; ++i) {
    // Generate node
    nodes.push_back(new Server());

    // Choose painlessMesh for testing
    nodes.back()->_mesh._selected_mesh_network_type =
        broth::meshnetwork::PAINLESSMESH;

    // Set node ID, start IDs from 1
    nodes.back()->_mesh.setNodeId(i + 1);

    // Initialize the node
    nodes.back()->init(MESH_NAME,
                       MESH_PASSWORD,
                       MESH_PORT,
                       broth::logger::DEBUG);
  }

  // Create the connections between nodes wihtin the virtual mesh network
  for(uint32_t i = 0; i < target_number_of_nodes; ++i) {
    for(uint32_t j = 0; j < target_number_of_nodes; ++j) {
      if(nodes[i]->_mesh._node_id != nodes[j]->_mesh._node_id) {
        // Add neighbour nodes except the node itself
        nodes[i]->_mesh.addNeighbourNode(nodes[j]->_mesh);
      }
    }
  }

  std::cout << "\n\033[95m>> Calling broth::server::Server::update() in a "
               "loop for all nodes at random order:\033[0m\n";

  uint32_t iteration = 0;
  bool printed_output = false;

  ///////////////////////////////////
  // Simulates loop() from Arduino //
  ///////////////////////////////////
  bool flag = true;
  while(true) {
    // Shuffle the order
    std::random_shuffle(nodes.begin(), nodes.end());

    // Iterate over the nodes based on the random order
    for(auto it = nodes.begin(); it != nodes.end(); ++it) {
      // clang-format off
      // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
      //////////////////////////////////////////////////////////////////////////


      // Call the update() function of the node
      (*it)->update();


      if((*it)->_state == LEADER){
        (*it)->_log.pushEntry(std::make_pair((*it)->_term, std::to_string((*it)->_mesh.getMeshTime())));
        std::cout << (*it)->_log._entries.size() << std::endl;

      }



      //////////////////////////////////////////////////////////////////////////
      // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      // clang-format on
      // Check if anything was outputted
      printed_output = (printed_output || (*it)->_logger._printed_output);

      // Reset the output flag for the specific node
      (*it)->_logger._printed_output = false;
    }

    flag = false;

    // Print the iteration number if anything was outputted to the terminal
    if((printed_output) && (iteration > 0)) {
      std::cout << "\033[95m^^ loop() iteration #" << iteration
                << " @ mesh time " << nodes.back()->_mesh.getMeshTime()
                << "\033[0m\n\n";
    }

    // Decide to stop the simulation or not
    if(nodes.back()->_mesh.getMeshTime() >
       (result["time"].as<float>() * 1000000)) {
      break;
    }

    printed_output = false;
    iteration++;
  }

  // auto run_node =
  //     [](uint32_t node_id, std::vector<Server*> nodes, uint32_t target_time)
  //     {
  //       while(true) {
  //         nodes[node_id]->update();

  //         if(nodes[node_id]->_mesh.getMeshTime() > target_time) {
  //           break;
  //         }
  //       }
  //     };

  // std::vector<std::thread> threads;

  // for(uint32_t i = 0; i < result["nodes"].as<int>(); ++i) {
  //   threads.push_back(std::thread(run_node,
  //                                 i,
  //                                 nodes,
  //                                 (result["time"].as<float>() * 1000000)));
  // }

  // for(uint32_t i = 0; i < result["nodes"].as<int>(); ++i) {
  //   threads[i].join();
  // }

  return 0;
}