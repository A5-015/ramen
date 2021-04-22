"""Mesh with Various Parameters Experiment Suite for Ramen

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
num_simulations = 10

# metrics
heart_beat_periods = [60, 120, 180, 240, 300, 360, 420, 480, 540, 600, 660, 720]
# election_max = [120, 180, 240, 300, 360, 420, 480, 540, 600, 660]
metric = heart_beat_periods

num_nodes = 50
down_time = 0

time_first_leader_data = [None] * len(metric)
time_first_leader_error = [None] * len(metric)
election_win_ratio = [None] * len(metric)
election_win_error = [None] * len(metric)


## MESH NETWORK
fig, axs = plt.subplots(1, 2, figsize=(12.8, 4.8))

# axs[0].set_ylim([0, 1.1])
axs[0].set_xlabel("Heartbeat Intervals")
axs[0].set_ylabel("Average Time to First Leader")
# axs[0].set_title(label="Packet Success Ratio Against Mesh Network Size")

# axs[1].set_ylim([-0.1, 1.1])
axs[1].set_xlabel("Heartbeat Intervals")
axs[1].set_ylabel("Average Election Win Ratio")
# axs[1].set_title(label="Election Win Ratio Against Mesh Network Size")

for h, i in zip(metric, range(len(metric))):

    print(
        "Simulating {0} mesh experiments with {1} nodes with a {2} hearbeat interval".format(
            num_simulations, num_nodes, h
        )
    )

    # HEART BEAT
    sim_config_json.main(
        [
            "-o",
            filename,
            "-t",
            "mesh",
            "-n",
            str(num_nodes),
            "--heartbeat",
            str(h),
        ]
    )

    # # ELECTION MAX
    # sim_config_json.main(
    #     [
    #         "-o",
    #         filename,
    #         "-t",
    #         "mesh",
    #         "-n",
    #         str(num_nodes),
    #         "--election_max",
    #         str(h),
    #     ]
    # )

    summary_table = run_coracle.main(
        ["-i", filename, "-e", str(num_simulations), "--pdf", False]
    )

    time_first_leader_data[i] = summary_table.loc["time to first leader"][0]
    time_first_leader_error[i] = summary_table.loc["time to first leader"][1]
    election_win_ratio[i] = summary_table.loc["election win ratio"][0]
    election_win_error[i] = summary_table.loc["election win ratio"][1]

axs[0].errorbar(
    metric,
    time_first_leader_data,
    fmt="o",
    yerr=time_first_leader_error,
    color="b",
    ms=np.sqrt(30),
    capsize=3,
    label="Mesh Network",
)

axs[1].errorbar(
    metric,
    election_win_ratio,
    fmt="o",
    yerr=election_win_error,
    color="b",
    ms=np.sqrt(30),
    capsize=3,
)

fig.suptitle(r"Mesh Network Simulation Results with Varying Raft Parameters")

# fig.legend(bbox_to_anchor=(0.0, 0.45, 0.5, 0.5), ncol=2)
fig.legend(loc=1, ncol=1)
plt.savefig("test " + "unit^2.png", dpi=300)

print("\nCompleted experiments for {0} heartbeat interval!\n".format(h))
