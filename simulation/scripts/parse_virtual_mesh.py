import re
import subprocess
from pprint import pprint

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import MaxNLocator
from scipy.stats import sem

one_sec = 1000000  # Constant, don't change

n_run_for_each = 10
time = 60


def main():
    n_nodes_test(n_nodes_to_test=[3, 5, 10, 20, 30, 50, 80, 100])
    # heart_beat_test(
    #     heart_beat_periods_to_test=[
    #         0.25 * one_sec,
    #         0.5 * one_sec,
    #         0.75 * one_sec,
    #         1 * one_sec,
    #         1.25 * one_sec,
    #         1.5 * one_sec,
    #     ],
    #     n_nodes=5,
    # )
    # election_timeout_test(
    #     election_timeouts_to_test=[
    #         [0.1 * one_sec, 0.2 * one_sec],
    #         [0.1 * one_sec, 0.3 * one_sec],
    #         [0.1 * one_sec, 0.4 * one_sec],
    #         [0.1 * one_sec, 0.5 * one_sec],
    #         [0.1 * one_sec, 0.6 * one_sec],
    #         [0.1 * one_sec, 0.7 * one_sec],
    #         [0.1 * one_sec, 0.8 * one_sec],
    #         [0.1 * one_sec, 0.9 * one_sec],
    #         [0.1 * one_sec, 1 * one_sec],
    #     ],
    #     n_nodes=5,
    # )


def election_timeout_test(election_timeouts_to_test, n_nodes):
    average_election_duration_results = []
    average_election_duration_results_err = []
    election_win_ratio_results = []
    election_win_ratio_results_err = []

    for election_timeout in election_timeouts_to_test:
        n_run_average_election_duration_results = []
        n_run_election_win_ratio_results = []

        # Run same test n_run_for_each times
        for n_run in range(n_run_for_each):

            election_timeout_min = int(election_timeout[0])
            election_timeout_max = int(election_timeout[1])

            print(
                f"Running the simulation with {n_nodes} node(s) for {time} second(s) with min election timout of {election_timeout_min} us and max election timeout of {election_timeout_max}"
            )

            average_election_duration, election_win_ratio = run_simulation(
                time=time,
                n_nodes=n_nodes,
                election_min=election_timeout_min,
                election_max=election_timeout_max,
            )

            print(f"Average time to win election: {average_election_duration}")
            print(
                f"Ratio of elections won those those started: {election_win_ratio}"
            )

            n_run_average_election_duration_results.append(
                average_election_duration
            )
            n_run_election_win_ratio_results.append(election_win_ratio)

        # Append the average of the runs
        average_election_duration_results.append(
            int(avg(n_run_average_election_duration_results))
        )

        # Calculate standard error
        average_election_duration_results_err.append(
            int(
                np.std(n_run_average_election_duration_results)
                / np.sqrt(np.size(n_run_average_election_duration_results))
            )
        )

        # Append the average of the runs
        election_win_ratio_results.append(
            int(avg(n_run_election_win_ratio_results))
        )

        # Calculate standard error
        election_win_ratio_results_err.append(
            int(
                np.std(n_run_election_win_ratio_results)
                / np.sqrt(np.size(n_run_election_win_ratio_results))
            )
        )

    pprint(
        {
            "heart_beat_plot_data": {
                "election_timeouts_to_test": election_timeouts_to_test,
                "time": time,
                "n_nodes": n_nodes,
                "average_election_duration_results": average_election_duration_results,
                "average_election_duration_results_err": average_election_duration_results_err,
                "election_win_ratio_results": election_win_ratio_results,
                "election_win_ratio_results_err": election_win_ratio_results_err,
            }
        }
    )

    plot(
        title_1="Average time to win election",
        title_2="Ratio of elections won those those started",
        x_1=[x[1] for x in election_timeouts_to_test],
        y_1=average_election_duration_results,
        y_1_err=average_election_duration_results_err,
        x_1_label="Election Timeout Period",
        y_1_label="Average Election Duration",
        x_2=[x[1] for x in election_timeouts_to_test],
        y_2=election_win_ratio_results,
        y_2_err=election_win_ratio_results_err,
        x_2_label="Election Timeout Period",
        y_2_label="Average Election Win Ratio",
        fig_name="election_timeout.png",
    )


def heart_beat_test(heart_beat_periods_to_test, n_nodes):
    average_election_duration_results = []
    average_election_duration_results_err = []
    election_win_ratio_results = []
    election_win_ratio_results_err = []

    for heart_beat_period in heart_beat_periods_to_test:
        n_run_average_election_duration_results = []
        n_run_election_win_ratio_results = []

        # Run same test n_run_for_each times
        for n_run in range(n_run_for_each):

            heart_beat_period = int(heart_beat_period)

            print(
                f"Running the simulation with {n_nodes} node(s) for {time} second(s) with HEART_BEAT_TIMER_PERIOD = {heart_beat_period} us"
            )

            define_flag = f"#define HEART_BEAT_TIMER_PERIOD {heart_beat_period}"

            average_election_duration, election_win_ratio = run_simulation(
                time=time,
                n_nodes=n_nodes,
                define_flag=define_flag,
            )

            print(f"Average time to win election: {average_election_duration}")
            print(
                f"Ratio of elections won those those started: {election_win_ratio}"
            )

            n_run_average_election_duration_results.append(
                average_election_duration
            )
            n_run_election_win_ratio_results.append(election_win_ratio)

        # Append the average of the runs
        average_election_duration_results.append(
            int(avg(n_run_average_election_duration_results))
        )

        # Calculate standard error
        average_election_duration_results_err.append(
            int(
                np.std(n_run_average_election_duration_results)
                / np.sqrt(np.size(n_run_average_election_duration_results))
            )
        )

        # Append the average of the runs
        election_win_ratio_results.append(
            int(avg(n_run_election_win_ratio_results))
        )

        # Calculate standard error
        election_win_ratio_results_err.append(
            int(
                np.std(n_run_election_win_ratio_results)
                / np.sqrt(np.size(n_run_election_win_ratio_results))
            )
        )

    pprint(
        {
            "heart_beat_plot_data": {
                "hear_beat_periods_to_test": heart_beat_periods_to_test,
                "time": time,
                "n_nodes": n_nodes,
                "average_election_duration_results": average_election_duration_results,
                "average_election_duration_results_err": average_election_duration_results_err,
                "election_win_ratio_results": election_win_ratio_results,
                "election_win_ratio_results_err": election_win_ratio_results_err,
            }
        }
    )

    plot(
        title_1="Average time to win election",
        title_2="Ratio of elections won those those started",
        x_1=heart_beat_periods_to_test,
        y_1=average_election_duration_results,
        y_1_err=average_election_duration_results_err,
        x_1_label="Heartbeat Period",
        y_1_label="Average Election Duration",
        x_2=heart_beat_periods_to_test,
        y_2=election_win_ratio_results,
        y_2_err=election_win_ratio_results_err,
        x_2_label="Heartbeat Period",
        y_2_label="Average Election Win Ratio",
        fig_name="heart_beat.png",
    )


def n_nodes_test(n_nodes_to_test):
    average_election_duration_results = []
    average_election_duration_results_err = []
    election_win_ratio_results = []
    election_win_ratio_results_err = []

    for n_nodes in n_nodes_to_test:
        n_run_average_election_duration_results = []
        n_run_election_win_ratio_results = []

        # Run same test n_run_for_each times
        for n_run in range(n_run_for_each):

            print(
                f"Running the simulation with {n_nodes} node(s) for {time} second(s)"
            )

            average_election_duration, election_win_ratio = run_simulation(
                time=time,
                n_nodes=n_nodes,
            )

            print(f"Average time to win election: {average_election_duration}")
            print(
                f"Ratio of elections won those those started: {election_win_ratio}"
            )

            n_run_average_election_duration_results.append(
                average_election_duration
            )
            n_run_election_win_ratio_results.append(election_win_ratio)

        # Append the average of the runs
        average_election_duration_results.append(
            int(avg(n_run_average_election_duration_results))
        )

        # Calculate standard error
        average_election_duration_results_err.append(
            int(
                np.std(n_run_average_election_duration_results)
                / np.sqrt(np.size(n_run_average_election_duration_results))
            )
        )

        # Append the average of the runs
        election_win_ratio_results.append(
            int(avg(n_run_election_win_ratio_results))
        )

        # Calculate standard error
        election_win_ratio_results_err.append(
            int(
                np.std(n_run_election_win_ratio_results)
                / np.sqrt(np.size(n_run_election_win_ratio_results))
            )
        )

    pprint(
        {
            "n_nodes_plot_data": {
                "time": time,
                "n_nodes_to_test": n_nodes_to_test,
                "average_election_duration_results": average_election_duration_results,
                "average_election_duration_results_err": average_election_duration_results_err,
                "election_win_ratio_results": election_win_ratio_results,
                "election_win_ratio_results_err": election_win_ratio_results_err,
            }
        }
    )

    plot(
        title_1="Average time to win election",
        title_2="Ratio of elections won those those started",
        x_1=n_nodes_to_test,
        y_1=average_election_duration_results,
        y_1_err=average_election_duration_results_err,
        x_1_label="Number of Nodes",
        y_1_label="Average Election Duration",
        x_2=n_nodes_to_test,
        y_2=election_win_ratio_results,
        y_2_err=election_win_ratio_results_err,
        x_2_label="Number of Nodes",
        y_2_label="Average Election Win Ratio",
        fig_name="n_nodes.png",
    )


def plot(
    title_1,
    title_2,
    x_1,
    y_1,
    y_1_err,
    x_1_label,
    y_1_label,
    x_2,
    y_2,
    y_2_err,
    x_2_label,
    y_2_label,
    fig_name,
):
    fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(12.8, 4.8))
    axes[0].set_title(title_1)
    axes[0].scatter(x_1, y_1)
    axes[0].errorbar(
        x_1,
        y_1,
        yerr=y_1_err,
        fmt="o",
        capsize=3,
    )
    axes[0].set_xlabel(x_1_label)
    axes[0].set_ylabel(y_1_label)

    axes[1].set_title(title_2)
    axes[1].scatter(x_2, y_2)
    axes[1].errorbar(
        x_2,
        y_2,
        yerr=y_2_err,
        fmt="o",
        capsize=3,
    )
    axes[1].set_xlabel(x_2_label)
    axes[1].set_ylabel(y_2_label)

    # fig.tight_layout()
    plt.savefig(fig_name, dpi=300)


def avg(lst):
    return sum(lst) / len(lst)


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


def run_simulation(
    time,
    n_nodes,
    kill_leader_at_time=0,
    n_logs_to_append=1,
    define_flag=None,
    election_min=None,
    election_max=None,
):
    """
    Runs ramen with given parameters
    """

    if define_flag is not None:
        try:
            # Reset the cpp file
            command = (
                f"cd ../../library && git checkout HEAD -- test/virtual_esp.cpp"
            )
            output = subprocess.check_output(command, shell=True, text=True)

            # Run sim
            command = f"cd ../../library && sed -i '1 i\{define_flag} /\/\ please remove me' test/virtual_esp.cpp && cmake . && make virtual_esp && ./bin/virtual_esp -t {time} -n {n_nodes} -l {n_logs_to_append} -k {kill_leader_at_time}"
            output = subprocess.check_output(command, shell=True, text=True)

        finally:
            # Reset the cpp file
            command = (
                f"cd ../../library && git checkout HEAD -- test/virtual_esp.cpp"
            )
            output = subprocess.check_output(command, shell=True, text=True)

    elif (election_min is not None) and (election_max is not None):
        try:
            # Reset the cpp file
            command = (
                f"cd ../../library && git checkout HEAD -- src/ramen/server.cpp"
            )
            output = subprocess.check_output(command, shell=True, text=True)

            # Run sim
            command = f'cd ../../library && sed -i "s/(min_val + (std::rand() % 10)) \* ELECTION_TIMEOUT_FACTOR/({election_min} + (std::rand() % {election_max}))/g" src/ramen/server.cpp && cmake . && make virtual_esp && ./bin/virtual_esp -t {time} -n {n_nodes} -l {n_logs_to_append} -k {kill_leader_at_time}'
            output = subprocess.check_output(command, shell=True, text=True)

        finally:
            # Reset the cpp file
            command = (
                f"cd ../../library && git checkout HEAD -- src/ramen/server.cpp"
            )
            output = subprocess.check_output(command, shell=True, text=True)

    else:
        # Reset the cpp file
        command = (
            f"cd ../../library && git checkout HEAD -- src/ramen/server.cpp"
        )
        output = subprocess.check_output(command, shell=True, text=True)

        # Reset the cpp file
        command = (
            f"cd ../../library && git checkout HEAD -- test/virtual_esp.cpp"
        )
        output = subprocess.check_output(command, shell=True, text=True)

        # Run sim
        command = f"cd ../../library && cmake . && make virtual_esp && ./bin/virtual_esp -t {time} -n {n_nodes} -l {n_logs_to_append} -k {kill_leader_at_time}"
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
            assert (
                last_election_item["lost_at"] > last_election_item["started_at"]
            )

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
            assert (
                last_election_item["won_at"] > last_election_item["started_at"]
            )

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

    if total_election_count == 0:
        election_win_ratio = 0
    else:
        election_win_ratio = (
            n_successful_election / total_election_count
        ) * 100

    return average_election_duration, election_win_ratio


if __name__ == "__main__":
    main()
