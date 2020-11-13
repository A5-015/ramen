#!/usr/bin/env python3

"""A simple script that takes network configuration parameters as an infile
and generates a JSON file for the Coracle Simulator

"""

import os
import sys
import argparse
import json
from network_generator import Mesh, Star

def _link_star_simple(num_nodes):

    links = list()

    for i in range(0, num_nodes-1):
        # full duplex
        temp_out = {}
        temp_out['start'] = i+1
        temp_out['end'] = (num_nodes)
        temp_out['id'] = 2*i+1
        temp_out['direction'] = "uni"
        links.append(temp_out)

        temp_in = {}
        temp_in['start'] = (num_nodes)
        temp_in['end'] = i+1
        temp_in['id'] = 2*i+2
        temp_in["direction"] = "uni"
        links.append(temp_in)

    return links

def _build_mesh(network, num_nodes, width, length, height):

    nodes = list()
    links = list()

    # use network generator API to obtain mesh network
    mesh = Mesh(x=width, y=length, z=height)
    mesh.add_nodes(n=num_nodes, max_range=350, min_range=300, max_link_budget=8, min_link_budget=4)

    mesh.find_neighbors()
    mesh.create_links()
    mesh.remove_disconnected_nodes()

    # @Barkin: these seem to be need to called, not sure why
    mesh.network_edges
    mesh.network_node_count

    # mesh.draw()

    # store each node as server and append to node list
    for i in range(0, mesh.network_node_count):
        temp = {}
        temp['type'] = "server"
        temp['id'] = i+1
        nodes.append(temp)

    # store each link as a bidirectional link and append to link list
    i = 1
    for key in mesh.network_edges:
        temp = {}
        temp['start'] = key[0]
        temp['end'] = key[1]
        temp['id'] = i
        temp['direction'] = "bi"

        i+=1
        links.append(temp)

    #  add node list and link list to network dictionary
    network['nodes'] = nodes
    network['links'] = links


def _link_star(network, num_nodes, width, length, height):

    nodes = list()
    links = list()

    star = Star(x=width, y=length, z=height)
    # Nodes have to have a link budget of 1 in the star network
    star.add_nodes(n=num_nodes, max_range=350, min_range=300, max_link_budget=1, min_link_budget=1)
    star.set_hub_constraints(n=num_nodes, max_range=250, min_range=200,
                             max_link_budget=250, min_link_budget=200)

    star.find_neighbors(percent_network_coverage=95, draw=False)
    star.place_hubs()
    star.create_links()
    star.remove_disconnected_nodes()

    # print(star.network_edges)
    # print(star.network_node_count)
    # print(star.network_hubs)

    # star.draw()

    #  add node list and link list to network dictionary
    network['nodes'] = nodes
    network['links'] = links

def create_system_dict(system_dict, args):

    system_dict['termination'] = args.termination

    # consensus configurations
    consensus = {}
    consensus['protocol'] = args.protocol
    consensus['election_timeout_min'] = args.election_min
    consensus['election_timeout_max'] = args.election_max
    consensus['heartbeat_interval'] = args.heartbeat

    system_dict['consensus'] = consensus

    return

def configure_network_parameters(system_dict, args):

    network = {}

    if (args.topology) == "mesh":
        _build_mesh(network, args.nodes, args.width, args.length, args.height)
    elif(args.topology) == "star":
        _link_star(network, args.nodes, args.width, args.length, args.height)
    else:
        exit()

    system_dict['network'] = network
    return

def initialize_network_nominal(system_dict, args):

    events = list()

    init_time = {}
    init_time['time'] = 0

    links = list()
    nodes = list()

    for i in range (0, len(system_dict['network']['links'])):
        temp = {}
        temp['id'] = i+1
        temp['type'] = 's'
        temp['active'] = True
        links.append(temp)

    for i in range(0, len(system_dict['network']['nodes'])):
        temp = {}
        temp['id'] = i+1
        temp['active'] = True
        nodes.append(temp)

    init_time['links'] = links
    init_time['nodes'] = nodes
    events.append(init_time)
    system_dict['network']['events'] = events

    # configure everything to start at time 0

def write_json(output_arg, system_dict):
    """
    Writes json file given a dictionary

    :param file output_arg: The output file to write to
    :param dict system_dict: The python dictionary to convert to JSON
    :return: None
    """

    json_object = json.dumps(system_dict, indent = 4)

    output_arg.write(json_object)

    return

def main(arguments):

    # argument parsing
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('nodes', help="The approximate number of nodes you desire to have in the network", type=int)
    parser.add_argument('--infile', help="Input file", type=argparse.FileType('r'))
    parser.add_argument('-o', '--outfile', help="Output file",
                        default=sys.stdout, type=argparse.FileType('w'))
    parser.add_argument('--termination', help="Termination time of simulation", default=1000)
    parser.add_argument('--protocol', help="The consensus algorithm", default="raft")
    parser.add_argument('--election_min', help="The minimum time for the election timeout", default=60)
    parser.add_argument('--election_max', help="The minimum time for the election timeout", default=300)
    parser.add_argument('--heartbeat', help="The heartbeat interval", default=30)
    parser.add_argument('-t', '--topology', help="The network topology", default="star")
    parser.add_argument('--width', help="The x dimension (width) of the virtual simulation space. Default is 5000m", default=1000)
    parser.add_argument('--length', help="The y dimension (length) of the virtual simulation space. Default is 5000m", default=1000)
    parser.add_argument('--height', help="The z dimension (height) of the virtual simulation space. Default is 0m", default=0)

    args = parser.parse_args(arguments)

    # create base simulation level dictionary
    system_dict = {}
    create_system_dict(system_dict, args)

    # configure network
    configure_network_parameters(system_dict, args)

    # initialize network
    initialize_network_nominal(system_dict, args)

    # write to JSON
    write_json(args.outfile, system_dict)

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
