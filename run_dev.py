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


def run_command_in_docker(command, privileged=False):
    """
    Runs the given command in the docker container

    :param command: Command to run
    :type command: str
    """

    if privileged:
        privileged_option = "--privileged"
    else:
        privileged_option = ""

    check_root_access()
    check_image()
    os.system(
        'docker run --user=1000 %s -t -v %s:/ramen -v %s:/.platformio ramen-dev /bin/bash -c "%s"'
        % (
            privileged_option,
            project_path,
            os.path.join(project_path, ".cache"),
            command,
        )
    )


def check_submodules():
    """
    Checks if the submodules are initialized
    """

    submodules = ["ArduinoJson", "TaskScheduler"]

    for module in submodules:
        if os.listdir(os.path.join(project_path, "library/test", module)) == []:
            print(
                "Initialize submodules first with 'git submodule update --init --recursive'"
            )
            exit()


if args.target == "catch":
    check_submodules()
    run_command_in_docker(
        "cd library && cmake . -DCMAKE_CXX_FLAGS='-Wall -Werror' && make VERBOSE=1 && run-parts --regex catch_ bin/"
    )

elif args.target == "shell" or args.target == "bash":
    check_root_access()
    check_image()
    os.system(
        "docker run --user=1000 -it -v %s:/ramen -v %s:/.platformio  ramen-dev"
        % (project_path, os.path.join(project_path, ".cache"))
    )

elif args.target == "clean":
    os.system("cd library && make clean")
    os.system(
        "cd library && find . -iwholename '*cmake*' -not -name CMakeLists.txt -delete"
    )
    os.system("rm -rf library/Makefile")
    os.system("rm -rf library/compile_commands.json")
    os.system("rm -rf library/bin")
    os.system("rm -rf library/.pio")
    os.system("rm -rf docs/html")
    print("Cleaned!")

elif args.target == "pio":
    run_command_in_docker(
        'platformio lib --global install painlessMesh && cd library && platformio ci --lib="." --board=nodemcuv2 examples/basic/basic.ino -O "build_flags = -Wall -Wextra -Wno-unused-parameter"'
    )

elif args.target == "doc" or args.target == "docs":
    run_command_in_docker("cd docs && doxygen")

elif args.target == "image" or args.target == "img":
    check_root_access()
    os.system("docker build -t ramen-dev .")

elif args.target == "upload":
    check_root_access()
    run_command_in_docker(
        "cd library/examples/basic && pio run --target upload", True
    )
    print(
        "^^^^ If you just saw `Permission denied: '/dev/ttyUSB0'` error above, run 'sudo chown ${USER}:dialout /dev/ttyUSB0' ^^^^"
    )
