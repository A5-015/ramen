#!/usr/bin/env python3

import argparse
import os
import pathlib
import subprocess

project_path = pathlib.Path(__file__).parent.absolute()

parser = argparse.ArgumentParser()
parser.add_argument("target")
args = parser.parse_args()


def check_image():
    """
    Checks if the image exists locally and builds it if it doesn't exist
    """

    ramen_dev_image = subprocess.check_output(
        ["docker", "images", "-q", "ramen-dev:latest"]
    ).decode("utf-8")

    if ramen_dev_image == "":
        os.system("docker build -t ramen-dev .")


def check_root_access():
    """
    Checks if the script has root privileges
    """

    if os.geteuid() != 0:
        print("Please run me with sudo or give me root access somehow")
        exit()


def run_command_in_docker(command):
    """
    Runs the given command in the docker container

    :param command: Command to run
    :type command: str
    """

    check_root_access()
    check_image()
    os.system(
        'docker run --user=1000 -v %s:/ramen ramen-dev /bin/bash -c "%s"'
        % (project_path, command)
    )

if args.target == "catch":
    run_command_in_docker(
        "cmake . -DCMAKE_CXX_FLAGS='-Wall -Werror' && make && run-parts --regex catch_ bin/"
    )

elif args.target == "shell" or args.target == "bash":
    check_image()
    os.system(
        "docker run --user=1000 -it -v %s:/ramen ramen-dev" % project_path
    )

elif args.target == "clean":
    os.system("make clean")
    os.system("find . -iwholename '*cmake*' -not -name CMakeLists.txt -delete")
    os.system("rm -rf bin")
    print("Cleaned!")

elif args.target == "pio":
    # NOTE: Running locally, not in docker
    os.system("platformio lib --global install painlessMesh")
    os.system(
        'platformio ci --lib="." --board=nodemcuv2 examples/basic/basic.ino -O "build_flags = -Wall -Wextra -Wno-unused-parameter"'
    )

elif args.target == "doc" or args.target == "docs":
    run_command_in_docker("cd docs && doxygen")
