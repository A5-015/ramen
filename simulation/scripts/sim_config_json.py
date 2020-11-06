#!/usr/bin/env python3

"""A simple script that takes network configuration parameters as an infile
and generates a JSON file for the Coracle Simulator

"""

import os
import sys
import argparse
import json

def _link_star(num_nodes):
    
    links = list()

    for i in range(0, num_nodes-1):
        # full duplex
        temp_out = {}
        temp_out['start'] = i+1
        temp_out['end'] = (num_nodes)
        temp_out['id'] = i+1
        temp_out['direction'] = "uni"
        links.append(temp_out)

        temp_in = {}
        temp_in['start'] = (num_nodes)
        temp_in['end'] = i+1
        temp_in['id'] = i+2
        temp_in["direction"] = "uni"
        links.append(temp_in)
    
    return links

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

    # instantiate the nodes
    nodes = list()

    for i in range(0,args.nodes):
        temp = {}
        temp['type'] = "server"
        temp['id'] = i+1
        nodes.append(temp)

    network['nodes'] = nodes

    # link the nodes
    if (args.topology) == "star":
        network['links'] = _link_star(args.nodes)
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
        temp['active'] = "true"
        links.append(temp)

    for i in range(0, args.nodes):
        temp = {}
        temp['id'] = i+1
        temp['active'] = "true"
        nodes.append(temp)

    init_time['links'] = links
    init_time['nodes'] = nodes
    events.append(init_time)
    system_dict['network']['events'] = events

    # configure everything to start at time 0

def write_json(output_arg, system_dict):

    json_object = json.dumps(system_dict, indent = 2)

    output_arg.write(json_object)

    return

def main(arguments):

    # argument parsing
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('nodes', help="Number of nodes", type=int)
    parser.add_argument('--infile', help="Input file", type=argparse.FileType('r'))
    parser.add_argument('-o', '--outfile', help="Output file",
                        default=sys.stdout, type=argparse.FileType('w'))
    parser.add_argument('--termination', help="Termination time of simulation", default=1000)
    parser.add_argument('--protocol', help="The consensus algorithm", default="raft")
    parser.add_argument('--election_min', help="The minimum time for the election timeout", default=60)
    parser.add_argument('--election_max', help="The minimum time for the election timeout", default=300)
    parser.add_argument('--heartbeat', help="The heartbeat interval", default=30)
    parser.add_argument('--topology', help="The network topology", default="star")
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
