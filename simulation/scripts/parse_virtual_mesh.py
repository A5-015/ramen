import re
import subprocess
from pprint import pprint

import matplotlib.pyplot as plt
import numpy as np

time = 10
kill_leader_at_time = 0
n_logs_to_append = 1


def main():

    n_nodes_to_test = [1, 2, 3, 4, 5, 10, 15, 20, 30]
    average_election_duration_results = []
    election_win_ratio_results = []

    for n_nodes in n_nodes_to_test:
        print(
            f"Running the simulation with {n_nodes} node(s) for {time} second(s) and will kill the leader at t={kill_leader_at_time}"
        )

        average_election_duration, election_win_ratio = run_simulation(
            time, kill_leader_at_time, n_nodes, n_logs_to_append
        )

        print(f"Average time to win election: {average_election_duration}")
        print(f"Ratio of elections won those those started: {election_win_ratio}")

        average_election_duration_results.append(average_election_duration)
        election_win_ratio_results.append(election_win_ratio)

    pprint(
        {
            "plot_data": {
                "n_logs_to_append": n_logs_to_append,
                "kill_leader_at_time": kill_leader_at_time,
                "time": time,
                "n_nodes_to_test": n_nodes_to_test,
                "average_election_duration_results": average_election_duration_results,
                "election_win_ratio_results": election_win_ratio_results,
            }
        }
    )

    fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(10, 6))
    axes[0].set_title("Average time to win election")
    axes[0].plot(n_nodes_to_test, average_election_duration_results)
    axes[1].set_title("Ratio of elections won those those started")
    axes[1].plot(n_nodes_to_test, election_win_ratio_results)
    fig.tight_layout()
    plt.savefig("test_n_nodes.png")


def get_node_id(line):
    """
    Parses a line like the following and returns 4 as the node id

    [ID:4][DEBUG] Current state: CANDIDATE @ 100001
    """
    pieces = line.split("[")
    for piece in pieces:
        if "ID:" in piece:
            return piece[3:-1]


def get_at_time(line):
    """
    Parses a line like the following and returns 100001 as the @ time

    [ID:4][DEBUG] Current state: CANDIDATE @ 100001
    """
    pieces = line.split("@")
    return pieces[1].strip().split(" ")[0].strip()


def run_simulation(time, kill_leader_at_time, n_nodes, n_logs_to_append):
    """
    Runs ramen with given parameters
    """
    # command = f"cd ../../library && cmake . && make virtual_esp && ./bin/virtual_esp -t {time} -n {n_nodes} -l {n_logs_to_append} -k {kill_leader_at_time}"
    # output = subprocess.check_output(command, shell=True, text=True)

    # f = open("output.txt", "r")
    # output = f.read()

    command = f"cd ../../library && ./bin/virtual_esp -t {time} -n {n_nodes} -l {n_logs_to_append} -k {kill_leader_at_time}"
    output = subprocess.check_output(command, shell=True, text=True)

    # Parse the output and get related lines
    result = re.findall("(.*(?: @ ).*)", output)

    # Initialize the empty dictionary
    all_elections = {}
    for n in range(1, n_nodes + 1):
        all_elections[n] = []

    for line in result:
        # Fill in election start times
        if "Started election" in line:
            node_id = int(get_node_id(line))
            election_started_at = int(get_at_time(line))
            all_elections[node_id].append(
                {
                    "started_at": election_started_at,
                    "lost_at": None,
                    "won_at": None,
                    "election_duration": None,
                    "success": False,
                }
            )

        # Fill in election lost times
        if "Did not win the election" in line:
            node_id = int(get_node_id(line))
            election_lost_at = int(get_at_time(line))

            last_election_item = all_elections[node_id][-1]
            last_election_item["lost_at"] = election_lost_at
            last_election_item["election_duration"] = (
                election_lost_at - last_election_item["started_at"]
            )
            all_elections[node_id][-1] = last_election_item

            # Make sure that the data makes sense
            assert last_election_item["lost_at"] > last_election_item["started_at"]

        # Fill in election won times
        if "Won the election" in line:
            node_id = int(get_node_id(line))
            election_won_at = int(get_at_time(line))

            last_election_item = all_elections[node_id][-1]
            last_election_item["success"] = True
            last_election_item["lost_at"] = None
            last_election_item["won_at"] = election_won_at
            last_election_item["election_duration"] = (
                election_won_at - last_election_item["started_at"]
            )
            all_elections[node_id][-1] = last_election_item

            # Make sure that the data makes sense
            assert last_election_item["won_at"] > last_election_item["started_at"]

    # Calculate the statistics
    average_election_duration = 0
    n_successful_election = 0
    total_election_count = 0
    for node, node_elections in all_elections.items():
        for election in node_elections:
            total_election_count += 1
            if election["success"]:
                n_successful_election += 1
                average_election_duration = (
                    average_election_duration + election["election_duration"]
                ) / 2

    election_win_ratio = (n_successful_election / total_election_count) * 100

    return average_election_duration, election_win_ratio


if __name__ == "__main__":
    main()
