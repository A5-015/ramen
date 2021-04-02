// #include "catch2/catch.hpp"
// #include "ramen.h"

// SCENARIO("There is only one node in the network") {
//   GIVEN("ramen init") {
//     ramen network;

//     // Choose painlessMesh for testing
//     network._mesh._selected_mesh_network_type =
//         broth::meshnetwork::PAINLESSMESH;

//     // Assign node id for testing
//     uint32_t test_node_id = 12345;

//     network._mesh.setNodeId(test_node_id);

//     network.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);

//     network._mesh.setNodeId(test_node_id);

//     REQUIRE(network._state == broth::server::FOLLOWER);
//     REQUIRE(network._term == 0);
//     REQUIRE(network._id == test_node_id);
//     REQUIRE(network._received_new_append_entry_request == false);
//     REQUIRE_FALSE(network._election_alarm == 0);

//     // Fast forward mesh time to trigger the existing election alarm
//     network._mesh.setMeshTime(1000000);

//     uint32_t term = 0;

//     WHEN("State is FOLLOWER") {
//       // Force the state as follower
//       network._state = broth::server::FOLLOWER;

//       for(uint32_t i = 0; i < RAFT_TIMER_PERIOD; i++) {
//         network.update();
//       }

//       THEN("term should be increased") {
//         REQUIRE(network._term > 10);
//       }

//       THEN("node should vote for itself") {
//         REQUIRE(network._voted_for == test_node_id);
//       }

//       THEN("node should become candidate") {
//         REQUIRE(network._state == broth::server::CANDIDATE);
//       }
//     }

//     WHEN("State is CANDIDATE") {
//       // Force the state as candidate
//       network._state = broth::server::CANDIDATE;

//       for(uint32_t i = 0; i < RAFT_TIMER_PERIOD; i++) {
//         network.update();
//       }

//       THEN("term should be increased") {
//         REQUIRE(network._term > 10);
//       }

//       THEN("node should vote for itself") {
//         REQUIRE(network._voted_for == test_node_id);
//       }

//       THEN("node should become candidate again") {
//         REQUIRE(network._state == broth::server::CANDIDATE);
//       }

//       THEN("request vote from others") {
//         // Check if _mesh.sendBroadcast() was called and the message itself
//       }
//     }
//   }
// }
