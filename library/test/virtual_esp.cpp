#include <unistd.h>

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
    ("l,log_length", "number of logs to append", cxxopts::value<int>()->default_value("5"))
    ("r,random", "run nodes in the random order", cxxopts::value<bool>()->default_value("false"))
    ("k,kill", "kill the leader at given time", cxxopts::value<int>()->default_value("0"))
    ;
  // clang-format on

  auto result = options.parse(argc, argv);

  uint32_t target_number_of_nodes = result["nodes"].as<int>();
  bool random_enabled = result["random"].as<bool>();
  uint32_t target_number_of_logs = result["log_length"].as<int>();
  uint32_t kill_leader_time = result["kill"].as<int>();

  std::vector<Server*> nodes;

  std::cout << "\n\033[95m>> Calling broth::server::Server::init() for all "
               "nodes:\033[0m\n";

  ////////////////////////////////////
  // Simulates setup() from Arduino //
  ////////////////////////////////////

  // Generate the nodes
  for(uint32_t i = 0; i < target_number_of_nodes; ++i) {
    // sleeps for 2 second
    unsigned int microsecond = 1000000;
    usleep(2 * microsecond);

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

  if(random_enabled) {
    std::cout << "\n\033[95m>> Running in randomized mode\033[0m\n";
  } else {
    std::cout << "\n\033[95m>> Running in non-randomized mode\033[0m\n";
  }

  if(kill_leader_time > 0) {
    std::cout << "\n\033[95m>> I'm going to kill the leader @@ "
              << kill_leader_time * 1000000 << " mesh time\033[0m\n";
  } else {
    std::cout << "\n\033[95m>> I'm not going to kill the leader\033[0m\n";
  }

  std::cout << "\033[95m>> Will append " << target_number_of_logs
            << " log(s) to the first leader\033[0m\n";

  std::cout << "\033[95m>> RAFT_TIMER_PERIOD is " << RAFT_TIMER_PERIOD
            << " \033[0m\n";

  std::cout << "\033[95m>> REQUEST_VOTE_TIMER_PERIOD is "
            << REQUEST_VOTE_TIMER_PERIOD << " \033[0m\n";

  std::cout << "\033[95m>> HEART_BEAT_TIMER_PERIOD is "
            << HEART_BEAT_TIMER_PERIOD << " \033[0m\n";

  std::cout << "\033[95m>> REQUEST_APPEND_ENTRY_PERIOD is "
            << REQUEST_APPEND_ENTRY_PERIOD << " \033[0m\n";

  std::cout << "\033[95m>> ELECTION_TIMEOUT_FACTOR is "
            << ELECTION_TIMEOUT_FACTOR << " \033[0m\n";

  std::cout << "\n\033[95m>> Calling broth::server::Server::update() in a "
               "loop for all nodes at random order:\033[0m\n";

  uint32_t iteration = 0;
  bool printed_output = false;

  ///////////////////////////////////
  // Simulates loop() from Arduino //
  ///////////////////////////////////
  int flag = 0;
  int kill_flag = (kill_leader_time > 0) ? 1 : 0;
  while(true) {
    // Shuffle the order
    if(random_enabled) {
      std::random_shuffle(nodes.begin(), nodes.end());
    }

    // Kill a leader at given time
    if(kill_flag) {
      if(nodes.back()->_mesh.getMeshTime() > (kill_leader_time * 1000000)) {
        for(auto it = nodes.begin(); it != nodes.end(); ++it) {
          if((*it)->getState() == LEADER) {
            uint32_t term = (*it)->_term;
            (*it)->switchState(FOLLOWER, term);
            kill_flag = 0;
            std::cout << "\033[95mJust killed leader " << (*it)->_id << " @ "
                      << (*it)->_mesh.getMeshTime() << " mesh time\033[0m\n";
          }
        }
      }
    }

    // Iterate over the nodes based on the random order
    for(auto it = nodes.begin(); it != nodes.end(); ++it) {
      // Call the update() function of the node
      (*it)->update();

      if((flag < target_number_of_logs) && (*it)->_state == LEADER) {
        std::cout << ">> Pushed data to my beloved leader" << std::endl;
        (*it)->distribute(std::to_string(flag), false);
        flag++;
      }

      // Check if anything was outputted
      printed_output = (printed_output || (*it)->_logger._printed_output);

      // Reset the output flag for the specific node
      (*it)->_logger._printed_output = false;
    }

    // Print the iteration number if anything was outputted to the terminal
    if((printed_output) && (iteration > 0)) {
      std::cout << "\033[95m^^ loop() iteration #" << iteration
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