#!/usr/bin/env python3

import argparse
import os
import pathlib
import subprocess

project_path = pathlib.Path(__file__).parent.absolute()
parser = argparse.ArgumentParser()

parser.add_argument(
    "--catch",
    action="store_true",
    help="Runs the catch tests available in the library/test/catch folder",
)

parser.add_argument(
    "--clean",
    action="store_true",
    help="Cleans the project folder by deleting the temporary files and other build artifacts",
)

parser.add_argument(
    "--pbuild",
    action="store_true",
    help="Builds the 'basic.ino' example",
)

parser.add_argument(
    "--pupload",
    action="store_true",
    help="Builds and uploads the 'basic.ino' example to the board",
)

parser.add_argument(
    "--pmonitor",
    action="store_true",
    help="Start a serial monitor",
)

parser.add_argument(
    "--pum",
    action="store_true",
    help="Builds, uploads the 'basic.ino' example to the board, and starts a serial monitor",
)

parser.add_argument(
    "--plist",
    action="store_true",
    help="Lists plugged development boards",
)

parser.add_argument(
    "--shell",
    action="store_true",
    help="Runs the Docker container in interactive mode and gives shell access",
)

parser.add_argument(
    "--img",
    action="store_true",
    help="Builds a new Docker image from the Dockerfile",
)

parser.add_argument(
    "--doc",
    action="store_true",
    help="Generates documentation from the doxygen comments",
)

parser.add_argument(
    "--init",
    action="store_true",
    help="Initialize and setup the repository for development",
)

args = parser.parse_args()


class bcolors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


tty_error_message = (
    bcolors.OKCYAN
    + "^^^^ If you just saw `Permission denied: '/dev/ttyUSB0'` error above, run 'sudo chown ${USER}:dialout /dev/ttyUSB0' ^^^^"
    + bcolors.ENDC
)


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
    :param privileged: Option to run the container in privileged mode, defaults to False
    :type privileged: bool, optional
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


if args.catch:
    check_submodules()
    run_command_in_docker(
        "cd library"
        + "&&"
        + "cmake . -DCMAKE_CXX_FLAGS='-Wall -Werror'"
        + "&&"
        + "make VERBOSE=1"
        + "&&"
        + "echo '\n\033[96m===============================================================================\nRunning the tests:\033[0m\n'"
        + "&&"
        + "run-parts --regex catch_ bin/"
    )

elif args.shell:
    check_root_access()
    check_image()
    os.system(
        "docker run --user=1000 -it -v %s:/ramen -v %s:/.platformio  ramen-dev"
        % (project_path, os.path.join(project_path, ".cache"))
    )

elif args.clean:
    os.system("cd library && make clean")
    os.system(
        "cd library"
        + "&&"
        + "find . -iwholename '*cmake*' -not -name CMakeLists.txt -delete"
    )
    os.system("rm -rf library/Makefile")
    os.system("rm -rf library/compile_commands.json")
    os.system("rm -rf library/bin")
    os.system("rm -rf library/.pio")
    os.system("rm -rf docs/html")
    print(bcolors.OKGREEN + "Cleaned!" + bcolors.ENDC)

elif args.pbuild:
    run_command_in_docker(
        "platformio lib --global install painlessMesh"
        + "&&"
        + "cd library"
        + "&&"
        + 'platformio ci --lib="." --board=nodemcuv2 examples/basic/basic.ino -O "build_flags = -Wall -Wextra -Wno-unused-parameter"'
    )

elif args.doc:
    run_command_in_docker("cd docs && doxygen")

elif args.img:
    check_root_access()
    os.system("docker build -t ramen-dev .")

elif args.pupload:
    check_root_access()
    run_command_in_docker(
        "cd library/examples/basic && pio run --target upload", True
    )
    print(tty_error_message)

elif args.pmonitor:
    check_root_access()
    run_command_in_docker("platformio device monitor --baud 115200", True)
    print(tty_error_message)

elif args.pum:
    check_root_access()
    run_command_in_docker(
        "cd library/examples/basic && pio run --target upload", True
    )
    run_command_in_docker("platformio device monitor --baud 115200", True)
    print(tty_error_message)

elif args.plist:
    check_root_access()
    run_command_in_docker("platformio device list", True)
    print(tty_error_message)

elif args.init:
    check_root_access()

    print(
        bcolors.OKGREEN
        + ">> Installing required system packages"
        + bcolors.ENDC
    )
    os.system("apt install -y -qq docker clang clang-format clang-tidy")

    print(
        bcolors.OKGREEN
        + ">> Installing required python packages"
        + bcolors.ENDC
    )
    os.system("pip3 install -U platformio black pylint")

    print(bcolors.OKGREEN + ">> Initialing the submodules" + bcolors.ENDC)
    os.system("git submodule update --init --recursive")

    print(
        bcolors.OKGREEN
        + ">> Checking out the correct version of the submodules"
        + bcolors.ENDC
    )
    os.system(
        "cd library/test/TaskScheduler"
        + "&&"
        + "git checkout b36cc818db89430b7564d1c56a668937f6dae6ec"
    )

    print(bcolors.OKGREEN + ">> DONE <<" + bcolors.ENDC)
