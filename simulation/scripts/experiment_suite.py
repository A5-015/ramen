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
from plotly.subplots import make_subplots
import matplotlib.pyplot as plt

import run_coracle
import sim_config_json

filename = "test.json"
num_simulations = 50
down_time = 500

# areas = [100, 200, 500, 700]
areas = [100]
num_nodes = [10, 20, 50, 100]


packet_success_data = [None] * len(num_nodes)
packet_success_error = [None] * len(num_nodes)
election_win_ratio = [None] * len(num_nodes)
election_win_error = [None] * len(num_nodes)

for a in areas:

    ## MESH NETWORK

    fig, axs = plt.subplots(1, 2, figsize=(12.8, 4.8))

    axs[0].set_ylim([0, 1.1])
    axs[0].set_xlabel("Number of Nodes")
    axs[0].set_ylabel("Average Packet Success Ratio")
    # axs[0].set_title(label="Packet Success Ratio Against Mesh Network Size")

    axs[1].set_ylim([-0.1, 1.1])
    axs[1].set_xlabel("Number of Nodes")
    axs[1].set_ylabel("Average Election Win Ratio")
    # axs[1].set_title(label="Election Win Ratio Against Mesh Network Size")

    for n, i in zip(num_nodes, range(len(num_nodes))):

        print(
            "Simulating {0} mesh experiments with {1} nodes in {2}x{2} area".format(
                num_simulations, n, a
            )
        )
        sim_config_json.main(
            [
                "-o",
                filename,
                "-t",
                "mesh",
                "-n",
                str(n),
                "--width",
                str(a),
                "--height",
                str(a),
                "--down_hub",
                str(down_time),
            ]
        )
        summary_table = run_coracle.main(
            ["-i", filename, "-e", str(num_simulations), "--pdf", False]
        )
        packet_success_data[i] = summary_table.loc["packet success ratio"][0]
        packet_success_error[i] = summary_table.loc["packet success ratio"][1]
        election_win_ratio[i] = summary_table.loc["election win ratio"][0]
        election_win_error[i] = summary_table.loc["election win ratio"][1]

    axs[0].errorbar(
        num_nodes,
        packet_success_data,
        fmt="o",
        yerr=packet_success_error,
        color="b",
        ms=np.sqrt(30),
        capsize=3,
    )

    axs[1].errorbar(
        num_nodes,
        election_win_ratio,
        fmt="o",
        yerr=election_win_error,
        color="g",
        ms=np.sqrt(30),
        capsize=3,
    )

    fig.suptitle(
        r"Mesh Network Simulation Results with Varying Network Sizes in a ${0} unit^2$ Area".format(
            a * a
        )
    )
    plt.savefig(str(a) + "unit^2_mesh.png", dpi=300)

    ## STAR NETWORK

    fig, axs = plt.subplots(1, 2, figsize=(12.8, 4.8))

    axs[0].set_ylim([0, 1.1])
    axs[0].set_xlabel("Number of Nodes")
    axs[0].set_ylabel("Average Packet Success Ratio")
    # axs[0].set_title(label="Packet Success Ratio Against Mesh Network Size")

    axs[1].set_ylim([-0.1, 1.1])
    axs[1].set_xlabel("Number of Nodes")
    axs[1].set_ylabel("Average Election Win Ratio")
    # axs[1].set_title(label="Election Win Ratio Against Mesh Network Size")

    for n, i in zip(num_nodes, range(len(num_nodes))):

        print(
            "Simulating {0} hub-spoke experiments with {1} nodes in {2}x{2} area".format(
                num_simulations, n, a
            )
        )
        sim_config_json.main(
            [
                "-o",
                filename,
                "-t",
                "star",
                "-n",
                str(n),
                "--width",
                str(a),
                "--height",
                str(a),
                "--down_hub",
                str(down_time),
            ]
        )
        summary_table = run_coracle.main(
            ["-i", filename, "-e", str(num_simulations), "--pdf", False]
        )
        packet_success_data[i] = summary_table.loc["packet success ratio"][0]
        packet_success_error[i] = summary_table.loc["packet success ratio"][1]
        election_win_ratio[i] = summary_table.loc["election win ratio"][0]
        election_win_error[i] = summary_table.loc["election win ratio"][1]

    axs[0].errorbar(
        num_nodes,
        packet_success_data,
        fmt="o",
        yerr=packet_success_error,
        color="b",
        ms=np.sqrt(30),
        capsize=3,
    )

    axs[1].errorbar(
        num_nodes,
        election_win_ratio,
        fmt="o",
        yerr=election_win_error,
        color="g",
        ms=np.sqrt(30),
        capsize=3,
    )

    fig.suptitle(
        r"Hub-Spoke Network Simulation Results with Varying Network Sizes in a ${0} unit^2$ Area".format(
            a * a
        )
    )
    plt.savefig(str(a) + "unit^2_star.png", dpi=300)

    print("\nCompleted experiments for {0}x{0} area!\n".format(a))
