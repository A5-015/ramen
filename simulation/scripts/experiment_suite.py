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

import numpy as np
import plotly.graph_objects as go

import run_coracle
import sim_config_json

filename = "test.json"
num_simulations = "1"
# num_nodes = ["5", "10", "20"]

num_nodes = ["5", "10", "20", "50", "100", "150", "200"]

packet_success_data = OrderedDict()

for n in num_nodes:

    print(
        "Simulating {0} experiments with {1} nodes".format(num_simulations, n)
    )
    sim_config_json.main(["-o", filename, "-t", "mesh", "-n", n])
    summary_table = run_coracle.main(
        ["-i", filename, "-e", num_simulations, "--pdf", False]
    )
    packet_success_data[n] = summary_table["packet success ratio"][0]

print(list(packet_success_data.keys()))
print(list(packet_success_data.values()))

fig = go.Figure()
fig.add_trace(
    go.Scatter(
        x=list(packet_success_data.keys()),
        y=list(packet_success_data.values()),
        mode="markers",
        marker=dict(
            size=16,
            colorscale="Viridis",  # one of plotly colorscales
        ),
    )
)
fig.update_layout(
    title="Average Packet Success Ratio Against Number of Nodes in the Network",
    xaxis_title="Average Packet Success Ratio",
    yaxis_title="Number of Nodes",
)

fig.show()
