/**
 * @file mesh_network.hpp
 * @brief mesh_network.hpp
 *
 */
#ifndef _RAMEN_MESH_NETWORK_HPP_
#define _RAMEN_MESH_NETWORK_HPP_

#include "ramen/configuration.hpp"
#include "ramen/logger.hpp"

namespace broth {
namespace meshnetwork {
  using namespace broth::logger;

  /**
   * @brief Holds the mesh network name mapping
   *
   */
  typedef enum { PAINLESSMESH = 0 } MeshNetworkType;

  /**
   * @brief Structure for defining the callback argument type
   *
   */
  typedef std::function<void(uint32_t from, string_t& data)>
      received_callback_t;

  /**
   * @brief Abstraction layer for the mesh network underneath
   *
   */
  class MeshNetwork {
   private:
    uint32_t _node_id;
    Scheduler _scheduler;
    Logger _logger;
    MeshNetworkType _selected_mesh_network_type;
    painlessMesh _painless_mesh;

   public:
    /**
     * @brief Construct a new Mesh Network object
     *
     */
    MeshNetwork();

    /**
     * @brief Initializes the choosen mesh network
     * Return true if the operation was successful, false otherwise
     *
     * @param mesh_network_type Type of the mesh network to initialize
     * @param mesh_name Name of the mesh network
     * @param mesh_password Password for the mesh network
     * @param mesh_port Port for the mesh network
     * @param logging_level Logging level for the system messages
     * @return true
     * @return false
     */
    bool init(MeshNetworkType mesh_network_type,
              string_t mesh_name,
              string_t mesh_password,
              uint16_t mesh_port,
              uint8_t logging_level = INFO);

    /**
     * @brief Calls the update function for the mesh network, if there is any
     * Return true if the operation was successful, false otherwise
     *
     * @return true
     * @return false
     */
    bool update();

    /**
     * @brief Get the ID of the current node
     *
     * @return uint32_t
     */
    uint32_t getNodeId();

    /**
     * @brief Get the current node time, as perceived by the mesh network
     *
     * @return uint32_t
     */
    uint32_t getNodeTime();

    /**
     * @brief Get the current mesh time (if different than node time), as
     * perceived by the mesh network
     *
     * @return uint32_t
     */
    uint32_t getMeshTime();

    /**
     * @brief Get the current list of nodes in the network
     *
     * @param include_self Whether to include the current node to the list or
     * not
     * @return std::list<uint32_t>
     */
    std::list<uint32_t> getNodeList(bool include_self = false);

    /**
     * @brief Send data to all nodes in the mesh network
     *
     * @param data Data to send
     * @return true
     * @return false
     */
    bool sendBroadcast(string_t data);

    /**
     * @brief Send data to a specific node with the given node ID
     *
     * @param destination_node_id The node to deliver the data to
     * @param data Data to send
     * @return true
     * @return false
     */
    bool sendMessageToNode(uint32_t destination_node_id, string_t data);

    /**
     * @brief The callback function which will be called when data is received
     * from another node
     *
     * @param on_receive A function that takes two arguments, you can use a
     * lambda function
     */
    void onReceiveCallback(received_callback_t on_receive);

    /////////////////////////////////////////////////
    // Methods used only during testing
    /////////////////////////////////////////////////

#ifdef _RAMEN_CATCH_TESTING_

    /**
     * @brief Force set the mesh time to given value
     * [NOTE: Used for testing purposes only]
     *
     * @param time
     */
    void setMeshTime(uint32_t time);

    /**
     * @brief Force increment the mesh time by given value
     * [NOTE: Used for testing purposes only]
     *
     * @param time
     */
    void incrementMeshTimeBy(uint32_t time);

    /**
     * @brief Force set the current node's ID
     * [NOTE: Used for testing purposes only]
     *
     * @param node_id
     */
    void setNodeId(uint32_t node_id);

    /**
     * @brief Force add a neighbour node to the current node
     * [NOTE: Used for testing purposes only]
     *
     * @param neighbour_node
     */
    void addNeighbourNode(MeshNetwork& neighbour_node);

    /**
     * @brief Go through the message buffer and see if there is any new message
     * [NOTE: Used for testing purposes only]
     */
    void checkForNewMessages();

#endif
  };

} // namespace meshnetwork
} // namespace broth

#endif