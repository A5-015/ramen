#!/usr/bin/env bash

# """Experiment Suite for Ramen

# This script explicitly lists out and executes the experiments conducted to test 
# * network stability
#     -defined as the percentage of packets successfully delivered to the destination (packet_success_ratio)

# * network realignment
#     -defined as the time taken for all of the nodes to arrive to a consensus after the introduction of a new node

# * network scalability
#     -defined as the number of nodes that can be a part of the network before deprecation of network quality within an area
z
# This script requires that the packages in requirements.txt be installed in your
# python environment
# """

# Network Stability

## Generate network file for 20 nodes
python sim_config_json.py \
-n 20 \
-o test.json

## Conduct an experiment with 50 runs
python run_coracle.py \
-i test.json \
-e 5

## Generate network file for 30 nodes
python sim_config_json.py \
-n 30 \
-o test.json

## Conduct an experiment with 50 runs
python run_coracle.py \
-i test.json \
-e 5

## Generate network file for 50 nodes
python sim_config_json.py \
-n 50 \
-o test.json

## Conduct an experiment with 50 runs
python run_coracle.py \
-i test.json \
-e 5

## Generate network file for 50 nodes
python sim_config_json.py \
-n 100 \
-o test.json

## Conduct an experiment with 50 runs
python run_coracle.py \
-i test.json \
-e 5

