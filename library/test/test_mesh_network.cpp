#include "catch2/catch.hpp"
#include "mesh_network.hpp"
#include "catch_common.hpp"

SCENARIO("Initialize Painlessmehs") {
    using namespace broth::meshnetwork;
    MeshNetwork network;

    network.init(PAINLESSMESH, MESH_NAME, MESH_PASSWORD, MESH_PORT);
}