#!/usr/bin/env python

"""Coracle Runner

This scripts runs the Coracle simulator using the configuration file
generated by sim_config_json.py and visualizes the result as a PDF report.

To view examples of the JSON files Coracle requires, visit:
https://github.com/consensus-oracle/coracle/tree/master/test

or execute `python sim_config_json.py -n 20 -o example_coracle.json` in this directory

This script generates a PDF report of the Coracle simulation using Plotly.

This script requires that the packages in requirements.txt be installed in your
python environment
"""

import argparse
import base64
import json
import multiprocessing
import os
import select
import subprocess
import sys
from collections import defaultdict
from multiprocessing.pool import ThreadPool

import numpy as np
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots
from tqdm import tqdm


class Visualizer:
    """
    Data visualization for Coracle simulation
    """

    def __init__(self):
        """
        Initialize class
        """
        self.raw_results = []
        self.raw_dictionary = defaultdict(list)
        # Define key
        self.psr = "packet success ratio"

    def _parse(self, result):
        """
        Parse the Coracle output

        :param result: Each run in the Coracle output
        :type result: string
        """
        # Store in ordered dictionary for easy retrieval
        for key, value in result.items():
            # Recurse to get to bottom of JSON tree
            if type(value) is dict:
                self._parse(value)
            else:
                # Only store statistics
                if key != "figures" and type(value) != str:
                    self.raw_dictionary[key].append(value)

    def _generate_statistics(self):
        """
        Generate statistics from Coracle results
        """

        # Insert packet success ratio into dictionary
        self.raw_dictionary[self.psr] = [
            r / d
            for r, d in zip(
                self.raw_dictionary["packets received"],
                self.raw_dictionary["packets dispatched"],
            )
        ]

        # Use dictionary length to inform dataframe size
        num_attributes = len(self.raw_dictionary)

        # Convert dictionary to pandas dataframe
        self.statistics_dataframe = pd.DataFrame(
            np.zeros((num_attributes, 2)),
            columns=["Mean", "Variance"],
            index=self.raw_dictionary.keys(),
        )

        # Calculate the mean and variance values
        for key, value in self.raw_dictionary.items():
            self.statistics_dataframe.loc[key, :][0] = np.mean(value)
            self.statistics_dataframe.loc[key, :][1] = np.var(value)

    def _generate_summary_table(self, fig):
        fig.add_trace(
            go.Table(
                header=dict(
                    values=list(["Attributes"])
                    + list(self.statistics_dataframe.columns)
                ),
                cells=dict(
                    values=[
                        self.statistics_dataframe.index,
                        self.statistics_dataframe.Mean,
                        self.statistics_dataframe.Variance,
                    ]
                ),
            ),
            row=1,
            col=1,
        )

    def _generate_packet_success_ratio_graph(self, fig):
        fig.add_trace(
            go.Scatter(
                x=[i + 1 for i in range(len(self.raw_dictionary[self.psr]))],
                y=self.raw_dictionary[self.psr],
            ),
            row=2,
            col=1,
        )
        fig.update_xaxes(title_text="Run", tickformat="d", row=2, col=1)
        fig.update_yaxes(title_text="Packet Success Ratio (%)", row=2, col=1)

    def initialize(self, all_runs):
        """
        Split the raw Coracle output by runs and iteratively parse

        :param all_runs: Raw coracle output
        :type all_runs: string
        """
        # Filter out to keep only results key in each run
        for run in all_runs:
            for key, value in run.items():
                if key == "results":
                    self.raw_results.append(value)
                    self._parse(value)

    def generate_pdf(self):
        """
        Generate PDF with Plotly to visualize data
        """
        # Calculate mean and variance from raw data
        self._generate_statistics()

        # Frame the plots for data visualization
        fig = make_subplots(
            rows=3,
            cols=1,
            column_widths=[1.0],
            row_heights=[1.0, 1.0, 1.0],
            specs=[
                [{"type": "table"}],
                [{"type": "scatter"}],
                [{"type": "scatter"}],
            ],
            subplot_titles=(
                "Summary of Statistics",
                "Packet Success Ratio per Run",
            ),
        )

        # Add summary table
        self._generate_summary_table(fig)

        # Add packet success plot
        self._generate_packet_success_ratio_graph(fig)

        # Configure overall layout
        fig.update_layout(
            height=2500, showlegend=False, title_text="Coracle Runs"
        )

        fig.show()


def call_proc(command):
    """
    Call a process at the OS level

    :param command: Command to execute in shell
    :type command: string
    :return: Subprocess
    :rtype: pointer
    """
    p = subprocess.Popen(
        ["/bin/bash", "-c", command],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    return p


def main(arguments):

    default_docker_image = "ghcr.io/a5-015/ramen/ramen-sim:docker"

    # Argument parsing
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("-i", "--infile", help="Input file", required=True)
    parser.add_argument(
        "-d",
        "--docker",
        help="Docker image to use",
        default=default_docker_image,
    )
    parser.add_argument(
        "-e",
        "--experiments",
        help="Number of experiments to run",
        type=int,
        default=1,
    )
    args = parser.parse_args(arguments)

    simulation_results = []
    processes = []

    with open(args.infile) as file:
        # Import the JSON file contents
        data = json.load(file)

        # Encode network simulation data into base64
        encoded_json = base64.b64encode(json.dumps(data).encode()).decode()

    # Build the command for each process
    command = (
        "docker run %s /bin/bash -c 'python3 base64_to_json.py %s > network.json && ./coracle/coracle_sim.byte -f network.json && echo' "
        % (args.docker, encoded_json)
    )

    # Run the simulation(s) process in parallel and append to processes list
    pool = ThreadPool(multiprocessing.cpu_count())
    for _ in range(args.experiments):
        processes.append(pool.apply_async(call_proc, (command,)).get())

    pool.close()

    # Store the stdout for each process in list
    streams = [p.stdout for p in processes]

    # Start a progress bar
    with tqdm(total=args.experiments, desc="Running simulations...") as pbar:
        while not all(p.poll() is not None for p in processes):
            try:
                rstreams, _, _ = select.select(streams, [], [])
                for stream in rstreams:
                    line = stream.readline()
                    if line != b"" and line != b"\n":
                        pbar.update(1)
                        simulation_results.append(line)
                pool.join()
            except KeyboardInterrupt:
                print("Terminating all simulation processes...")
                for p in processes:
                    p.kill()

    # Strip each result of a newline character using list comprehension
    simulation_results = [
        json.loads(result.strip().decode()) for result in simulation_results
    ]

    # with open(args.infile) as file:
    #     simulation_results = json.load(file)

    visualizer = Visualizer()
    visualizer.initialize(simulation_results)
    visualizer.generate_pdf()


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
