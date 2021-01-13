#!/usr/bin/env python

"""Runs the Coracle simulator using the configuration file
generated by sim_config_json.py and visualizes the result as a PDF report
"""

import os
import sys
import json
import base64
import argparse
import subprocess
import numpy as np
import pandas as pd
from tqdm import tqdm
import plotly.graph_objects as go
from plotly.subplots import make_subplots
from collections import defaultdict


class Visualizer:
    def __init__(self):
        self.raw_results = []
        self.raw_dictionary = defaultdict(list)
        # Define key
        self.psr = "packet success ratio"

    def _parse(self, result):
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

    def initialize(self, all_runs):
        # Filter out to keep only results key in each run
        for run in all_runs:
            for key, value in run.items():
                if key == "results":
                    self.raw_results.append(value)
                    self._parse(value)

    def generate_pdf(self):
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
            subplot_titles=("Summary of Statistics", "Packet Success Ratio per Run"),
        )

        # Add table
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

        # Add packet success plot
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

        # Configure overall layout
        fig.update_layout(height=2500, showlegend=False, title_text="Coracle Runs")

        fig.show()


def main(arguments):

    default_docker_image = "ghcr.io/a5-015/ramen/ramen-sim:docker"

    # Argument parsing
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("-i", "--infile", help="Input file", required=True)
    parser.add_argument(
        "-d", "--docker", help="Docker image to use", default=default_docker_image
    )
    parser.add_argument(
        "-e", "--experiments", help="Number of experiments to run", type=int, default=3
    )
    args = parser.parse_args(arguments)

    simulation_results = []
    with open(args.infile) as file:
        # Import the JSON file contents
        data = json.load(file)

        # Encode network simulation data into base64
        encoded_json = base64.b64encode(json.dumps(data).encode()).decode()

        # Construct the command chain for e loops
        command = (
            "sudo docker run %s /bin/bash -c 'for i in {1..%s}; do python3 base64_to_json.py %s > network.json; ./coracle/coracle_sim.byte -f network.json; echo; done'"
            % (args.docker, args.experiments, encoded_json)
        )

        # Run the simulation(s) process
        process = subprocess.Popen(
            ["/bin/bash", "-c", command],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

        # Start a progress bar
        with tqdm(total=args.experiments, desc="Running simulations...") as pbar:
            while process.poll() is None:
                line = process.stdout.readline()
                if line != b"" and line != b"\n":
                    pbar.update(1)
                    simulation_results.append(line)

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
