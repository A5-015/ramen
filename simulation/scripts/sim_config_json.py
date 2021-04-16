#!/usr/bin/env python

"""Simulation Configuration JSON Generator

This script takes arguments as configuration parameters. To view the input
arguments execute `python sim_config_json -h`

Number of nodes is a required argument. Please note that the modelling script for
generating nodes and connections is probabilistic. Hence, the configuration will not
necessarily generate exactly the same number of nodes provided, although it will be close.

This script generates the JSON configuration needed as the input for the
Coracle simulator (https://github.com/consensus-oracle/coracle).

If an outfile is not explicitly provided, the output will be printed to the terminal.
If a provided outfile does not exist, the script will generate a JSON file of that name.

This script requires that the packages in requirements.txt be installed in your
python environment
"""

import argparse
import json
import os
import sys
import random

from network_generator import Mesh, Star


def _link_star_simple(num_nodes):
    """


    :param num_nodes: Number of nodes in hub-spoke network
    :type num_nodes: int
    :return: List of hub-spoke links between the specified number of nodes
    :rtype: list
    """

    links = list()

    for i in range(0, num_nodes - 1):
        # full duplex
        temp_out = {}
        temp_out["start"] = i + 1
        temp_out["end"] = num_nodes
        temp_out["id"] = 2 * i + 1
        temp_out["direction"] = "uni"
        links.append(temp_out)

        temp_in = {}
        temp_in["start"] = num_nodes
        temp_in["end"] = i + 1
        temp_in["id"] = 2 * i + 2
        temp_in["direction"] = "uni"
        links.append(temp_in)

    return links


def _build_mesh(network, num_nodes, width, length, height):
    """
    Build a mesh network using network_generator.py

    :param network: Stores the network information
    :type network: dict
    :param num_nodes: Approximate number of nodes in mesh network
    :type num_nodes: int
    :param width: Width of the 3D space where the mesh network will be modelled
    :type width: int
    :param length: Length of the 3d space where the mesh network will be modelled
    :type length: int
    :param height: Width of the 3D space where the mesh network will be modelled
    :type height: int
    """

    nodes = list()
    links = list()

    # use network generator API to obtain mesh network
    mesh = Mesh(x=width, y=length, z=height)
    mesh.add_nodes(
        n=num_nodes,
        max_range=350,
        min_range=300,
        max_link_budget=4,
        min_link_budget=3,
    )

    mesh.find_neighbors()
    mesh.create_links()
    mesh.remove_disconnected_nodes()

    # print(mesh.network_edges)
    # print(mesh.network_node_count)
    # print(mesh.node_list)

    # store each link as a bidirectional link and append to link list
    i = 1
    for key in mesh.network_edges:
        temp = {}
        temp["start"] = key[0]
        temp["end"] = key[1]
        temp["id"] = i
        temp["direction"] = "bi"

        i += 1
        links.append(temp)

    # store each node as hub and append to node list
    for node in mesh.node_list:
        temp = {}
        temp["type"] = "hub"
        temp["id"] = node
        nodes.append(temp)

    # create a server node for each hub and link them bidirectionally
    for node in mesh.node_list:
        # initialize dicts
        temp_node = {}
        temp_link = {}

        # create server nodes with ids picked up from hub nodes
        temp_node["type"] = "server"
        temp_node["id"] = node + list(mesh.node_list)[-1]
        nodes.append(temp_node)

        # create links between server and corresponding hub
        temp_link["start"] = node
        temp_link["end"] = node + list(mesh.node_list)[-1]
        temp_link["id"] = i
        temp_link["direction"] = "bi"

        i += 1
        links.append(temp_link)

    #  add node list and link list to network dictionary
    network["nodes"] = nodes
    network["links"] = links
    # mesh.draw()


def _build_star(network, num_nodes, width, length, height):
    """
    Build a hub-spoke network using network_generator.py

    :param network: Stores the network information
    :type network: dict
    :param num_nodes: Approximate number of nodes in hub-spoke network
    :type num_nodes: int
    :param width: Width of the 3D space where the hub-spoke network will be modelled
    :type width: int
    :param length: Length of the 3d space where the hub-spoke network will be modelled
    :type length: int
    :param height: Width of the 3D space where the hub-spoke network will be modelled
    :type height: int
    """

    nodes = list()
    links = list()

    star = Star(x=width, y=length, z=height)
    # Nodes have to have a link budget of 1 in the star network
    star.add_nodes(
        n=num_nodes,
        max_range=350,
        min_range=300,
        max_link_budget=1,
        min_link_budget=1,
    )
    star.set_hub_constraints(
        n=num_nodes,
        max_range=250,
        min_range=200,
        max_link_budget=250,
        min_link_budget=200,
    )

    star.find_neighbors(percent_network_coverage=95, draw=False)
    star.place_hubs()
    star.create_links()
    star.remove_disconnected_nodes()

    # store each link as a bidirectional link and append to link list
    i = 1
    for key in star.network_edges:
        temp = {}
        temp["start"] = key[0]
        temp["end"] = key[1]
        temp["id"] = i
        temp["direction"] = "bi"

        i += 1
        links.append(temp)

    server_nodes = list(star.node_list - star.network_hubs)

    # store each non-hub node as server and append to node list
    for node in server_nodes:
        temp = {}
        temp["type"] = "server"
        temp["id"] = node
        nodes.append(temp)

    # store hub nodeas hub and append to node list
    for node in star.network_hubs:
        temp = {}
        temp["type"] = "hub"
        temp["id"] = node
        nodes.append(temp)

    # print(star.network_edges)
    # print(star.node_list)
    # print(star.network_hubs)
    # print(list(server_nodes))
    # star.draw()

    #  add node list and link list to network dictionary
    network["nodes"] = nodes
    network["links"] = links


def create_system_dict(system_dict, args):
    """
    Initializes the dictionary that will contain all the configuration

    :param system_dict: Dictionary that holds all configuration
    :type system_dict: dict
    :param args: Arguments provided by the user
    :type args: namespace
    """

    system_dict["termination"] = args.termination

    # consensus configurations
    consensus = {}
    consensus["protocol"] = args.protocol
    consensus["election_timeout_min"] = args.election_min
    consensus["election_timeout_max"] = args.election_max
    consensus["heartbeat_interval"] = args.heartbeat

    system_dict["consensus"] = consensus

    return


def configure_network_parameters(system_dict, args):
    """
    Calls the relevant network building functions given the topology specified by the user

    :param system_dict: Dictionary that holds all configuration
    :type system_dict: dict
    :param args: Arguments provided by the user
    :type args: namespace
    :raises Exception: Invalid topology if user specifies neither "mesh" nor "star"
    """

    network = {}

    if (args.topology) == "mesh":
        _build_mesh(network, args.nodes, args.width, args.length, args.height)
    elif (args.topology) == "star":
        _build_star(network, args.nodes, args.width, args.length, args.height)
    else:
        raise Exception("Invalid topology")
        exit()

    system_dict["network"] = network
    return


def initialize_network_nominal(system_dict, args):
    """
    Connects all nodes at time 0

    :param system_dict: Dictionary that holds all configuration
    :type system_dict: dict
    :param args: Arguments provided by the user
    :type args: namespace
    """

    events = list()

    init_time = {}
    init_time["time"] = 0

    links = list()
    nodes = list()

    for i in range(0, len(system_dict["network"]["links"])):
        temp = {}
        temp["id"] = i + 1
        temp["type"] = "s"
        temp["active"] = True
        links.append(temp)

    for n in system_dict["network"]["nodes"]:
        temp = {}
        temp["id"] = n["id"]
        temp["active"] = True
        nodes.append(temp)

    init_time["links"] = links
    init_time["nodes"] = nodes
    events.append(init_time)
    system_dict["network"]["events"] = events

    # configure everything to start at time 0


def down_hub_event(system_dict, down_hub_time, termination_time):
    """Creates a Coracle event to bring down a hub, which is eventually brought back up 200
    units of time later.

    :param system_dict: Dictionary that holds all configuration
    :type system_dict: dict
    :param down_hub_time: The time at which to bring down a hub
    :type down_hub_time: int
    :param termination_time: The termination time of the Coracle simulation
    :type termination_time: int
    """

    if down_hub_time + 200 > termination_time:
        return

    down_event = {}
    down_event["time"] = down_hub_time

    links = list()
    nodes = list()

    # find a hub
    hubs = []
    for n in system_dict["network"]["nodes"]:
        temp = {}
        temp["id"] = n["id"]
        temp["active"] = True
        if n["type"] == "hub":
            hubs.append(n["id"])
        nodes.append(temp)
    hub_id = random.choice(hubs)

    # cut all links of that hub
    for i in range(0, len(system_dict["network"]["links"])):
        temp = {}
        temp["id"] = i + 1
        temp["type"] = "s"

        if (
            system_dict["network"]["links"][i]["start"] == hub_id
            or system_dict["network"]["links"][i]["end"] == hub_id
        ):
            temp["active"] = False
        else:
            temp["active"] = True
        links.append(temp)

    down_event["links"] = links
    down_event["nodes"] = nodes
    system_dict["network"]["events"].append(down_event)

    # Bring all links back
    up_event = {}
    up_event["time"] = down_hub_time + 200

    links = list()
    nodes = list()

    for n in system_dict["network"]["nodes"]:
        temp = {}
        temp["id"] = n["id"]
        temp["active"] = True
        nodes.append(temp)

    for i in range(0, len(system_dict["network"]["links"])):
        temp = {}
        temp["id"] = i + 1
        temp["type"] = "s"
        temp["active"] = True
        links.append(temp)

    up_event["links"] = links
    up_event["nodes"] = nodes
    system_dict["network"]["events"].append(up_event)


def write_json(output_arg, system_dict):
    """
    Writes json file given a dictionary

    :param file output_arg: The output file to write to
    :param dict system_dict: The python dictionary to convert to JSON
    :return: None
    """

    json_object = json.dumps(system_dict, indent=4)

    output_arg.write(json_object)

    return


def main(arguments):

    # argument parsing
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "-n",
        "--nodes",
        help="The approximate number of nodes you desire to have in the network",
        type=int,
        required=True,
    )
    parser.add_argument("--infile", help="Input file", type=argparse.FileType("r"))
    parser.add_argument(
        "-o",
        "--outfile",
        help="Output file",
        default=sys.stdout,
        type=argparse.FileType("w"),
    )
    parser.add_argument(
        "--termination", help="Termination time of simulation", default=1000
    )
    parser.add_argument("--protocol", help="The consensus algorithm", default="raft")
    parser.add_argument(
        "--election_min",
        help="The minimum time for the election timeout",
        default=60,
    )
    parser.add_argument(
        "--election_max",
        help="The minimum time for the election timeout",
        default=300,
    )
    parser.add_argument("--heartbeat", help="The heartbeat interval", default=30)
    parser.add_argument("-t", "--topology", help="The network topology", default="mesh")
    parser.add_argument(
        "--width",
        type=float,
        help="The x dimension (width) of the virtual simulation space.",
        default=200,
    )
    parser.add_argument(
        "--length",
        type=float,
        help="The y dimension (length) of the virtual simulation space.",
        default=200,
    )
    parser.add_argument(
        "--height",
        type=float,
        help="The z dimension (height) of the virtual simulation space.",
        default=0,
    )
    parser.add_argument(
        "--down_hub",
        help="Down a hub at given time. If value is 0, then no hub will be downed.",
        type=int,
        default=0,
    )

    args = parser.parse_args(arguments)

    # create base simulation level dictionary
    system_dict = {}
    create_system_dict(system_dict, args)

    # configure network
    configure_network_parameters(system_dict, args)

    # initialize network
    initialize_network_nominal(system_dict, args)

    if args.down_hub:
        down_hub_event(system_dict, args.down_hub, args.termination)

    # write to JSON
    write_json(args.outfile, system_dict)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
