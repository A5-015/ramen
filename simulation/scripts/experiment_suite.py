"""Experiment Suite for Ramen

This script explicitly lists out and executes the experiments conducted to test
* network stability
    -defined as the percentage of packets successfully delivered to the destination (packet_success_ratio)

* network realignment
    -defined as the time taken for all of the nodes to arrive to a consensus after the introduction of a new node

* network scalability
    -defined as the number of nodes that can be a part of the network before deprecation of network quality within an area

This script requires that the packages in requirements.txt be installed in your
python environment
"""

import subprocess
from collections import OrderedDict 

ordered_experiments = OrdereDict{
    [""]
}
