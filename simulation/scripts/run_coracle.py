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
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


class Visualizer:
    def __init__(self):
        self.tables = []
        self.figures = []

    def parse(self, results):
        for key, value in results.items():

            if key == "table":
                self.tables.append(value)

            elif key == "figures":
                self.figures.append(value)

            if type(value) is dict:
                self.parse(value)

    def build_figure(self, fig, ax, raw_figure):

        ax.set(
            xlabel=raw_figure["x axis"]["label"],
            xlim=(raw_figure["x axis"]["start"], raw_figure["x axis"]["end"]),
            ylabel=raw_figure["y axis"]["label"],
            ylim=(raw_figure["y axis"]["start"], raw_figure["y axis"]["end"]),
            title=raw_figure["title"],
        )

        for line in raw_figure["data"]:
            x_points = []
            y_points = []
            for data_point in line["data"]:
                x_points.append(data_point["x"])
                y_points.append(data_point["y"])
            ax.plot(x_points, y_points)

    def generate_pdf(self):

        with PdfPages("./test.pdf") as export_pdf:
            for table in self.tables:
                df = pd.DataFrame.from_dict(table, orient="index")
                fig, ax = plt.subplots(figsize=(10, 6))
                plt.axis("off")
                pd.plotting.table(ax, df, loc="center")
                fig.tight_layout()
                export_pdf.savefig()
                plt.close()

            for raw_figure_set in self.figures:
                if raw_figure_set:
                    for raw_figure in raw_figure_set:
                        fig, ax = plt.subplots(figsize=(10, 6))
                        self.build_figure(fig, ax, raw_figure)
                        export_pdf.savefig()
                        plt.close()


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
        "-e", "--experiments", help="Number of experiments to run", default=5
    )
    args = parser.parse_args(arguments)

    with open(args.infile) as file:
        # Import the JSON file contents
        data = json.load(file)

        # Encode network simulation data into base64
        encoded_json = base64.b64encode(json.dumps(data).encode()).decode()

        # Construct the command chain
        command = (
            'sudo docker run %s /bin/sh -c "python3 base64_to_json.py %s > network.json && ./coracle/coracle_sim.byte -f network.json"'
            % (args.docker, encoded_json)
        )

        # Run the simulation
        simulation_result = subprocess.check_output(command, shell=True, text=True)

    # Parse the results
    raw_out = json.loads(simulation_result)
    raw_res = raw_out["results"]

    visualizer = Visualizer()
    visualizer.parse(raw_res)
    visualizer.generate_pdf()


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
