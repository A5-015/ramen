#!/usr/bin/env python3

import argparse
import os
import pathlib
import subprocess

parser = argparse.ArgumentParser()

parser.add_argument(
    "--test",
    action="store_true",
    help="Runs the unit tests available in the library/test folder",
)

parser.add_argument(
    "--virtual",
    action="store_true",
    help="Runs the library in a simulated environment",
)

parser.add_argument(
    "-n",
    type=int,
    help="Number of nodes to run in the library in a simulated environment. Needs to be used in combination with --virtual",
)

parser.add_argument(
    "-t",
    type=float,
    help="Duration of the simulation in the simulated environment in seconds. You can specify float values like 0.1 seconds. Needs to be used in combination with --virtual",
)

parser.add_argument(
    "-l",
    type=int,
    help="Number of logs to append to the leader. Needs to be used in combination with --virtual",
)

parser.add_argument(
    "-r",
    action="store_true",
    help="Randomizes the order of nodes while running the simulation. Needs to be used in combination with --virtual",
)

parser.add_argument(
    "-k",
    type=int,
    help="Sets leader kill time. Needs to be used in combination with --virtual",
)

parser.add_argument(
    "-p",
    type=int,
    help="The USB port to use, enter a number starting from 0. Needs to be used in combination with --pupload",
)

parser.add_argument(
    "-e",
    type=str,
    help="The example to build, enter its name without the ino extension. For example 'basic'. Needs to be used in combination with --pupload",
)

parser.add_argument(
    "--clean",
    action="store_true",
    help="Cleans the project folder by deleting the temporary files and other build artifacts",
)

parser.add_argument(
    "--pbuild",
    action="store_true",
    help="Builds the 'basic.ino' example only to make sure that there are no compilation errors in the library",
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
    "--shell",
    action="store_true",
    help="Runs the Docker container in interactive mode and gives shell access",
)

parser.add_argument(
    "--docs",
    action="store_true",
    help="Generates documentation from the doxygen comments",
)

parser.add_argument(
    "--init",
    action="store_true",
    help="Initialize and setup the repository for development",
)


class RunDev:
    def __init__(self, args):
        self.project_path = pathlib.Path(__file__).parent.absolute()

        self.submodules = ["ArduinoJson", "TaskScheduler"]

        self.color = {
            "HEADER": "\033[95m",
            "OKBLUE": "\033[94m",
            "OKCYAN": "\033[96m",
            "OKGREEN": "\033[92m",
            "WARNING": "\033[93m",
            "FAIL": "\033[91m",
            "ENDC": "\033[0m",
            "BOLD": "\033[1m",
            "UNDERLINE": "\033[4m",
        }

        if not args.init:
            self.check_submodules()

        if args.test:
            self.module_test()

        elif args.virtual:
            if (args.t is None) or (args.n is None) or (args.l is None):
                print("Please specify: ")
                print("> Number of nodes with -n argument")
                print("> Simulation time in seconds with -t argument")
                print("> Leader kill time in seconds with -k argument")
                print(
                    "> Number of logs to append to the leader with -l argument"
                )
                print("> To shuffle the node order with -r argument")
                self.exit_code()
            self.module_virtual(args.t, args.n, args.r, args.l, args.k)

        elif args.shell:
            self.module_shell()

        elif args.clean:
            self.module_clean()

        elif args.docs:
            self.module_docs()

        elif args.pbuild:
            # if args.e is None:
            #     print("Please specify which example to build with -e argument")
            #     self.exit_code()
            self.module_build()

        elif args.pupload:
            if args.e is None:
                print("Please specify which example to build with -e argument")
                self.exit_code()
            self.module_upload(example=args.e)

        elif args.pmonitor:
            if args.e is None:
                print(
                    "Please specify which example is currently running with -e argument"
                )
                print("^ this is needed for stack tracing and error decoding")
                self.exit_code()
            self.module_monitor(example=args.e)

        elif args.pum:
            if args.e is None:
                print("Please specify which example to build with -e argument")
                self.exit_code()
            self.module_upload(example=args.e)
            self.module_monitor(example=args.e)

        elif args.init:
            self.module_init()

    def module_monitor(self, example="basic", port=0):
        self.chown_ttyUSBX_to_user(port)
        self.run_command_in_docker(
            f"cd library/examples/{example} && platformio device monitor --baud 115200 --filter esp8266_exception_decoder",
            True,
        )

    def module_upload(self, example="basic", port=0):
        self.chown_ttyUSBX_to_user(port)
        print(
            self.color["OKCYAN"]
            + ">> 'Uploading %s example'" % example
            + self.color["ENDC"]
        )
        self.run_command_in_docker(
            f"cd library/examples/{example} && pio run --target upload", True
        )

    def module_build(self, example="basic"):
        print(
            self.color["OKCYAN"]
            + ">> 'Building %s example'" % example
            + self.color["ENDC"]
        )
        self.run_command_in_docker(
            "platformio lib --global install painlessMesh 'Adafruit BusIO' 'Adafruit GFX Library' 'Adafruit SSD1306' PCA9635"
            + "&&"
            + "cd library"
            + "&&"
            + f'platformio ci --lib="." --board=nodemcuv2 examples/{example}/{example}.ino -O "build_flags = -Wall -Wextra -Wno-unused-parameter"'
        )

    def module_docs(self):
        self.run_command_in_docker("cd docs && doxygen")

    def module_clean(self):
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
        print(self.color["OKGREEN"] + "Cleaned!" + self.color["ENDC"])

    def module_shell(self):
        self.check_root_access()
        os.system(
            "docker run --user=1000 -it -v %s:/ramen -v %s:/.platformio  ghcr.io/a5-015/ramen/ramen-dev"
            % (self.project_path, os.path.join(self.project_path, ".cache"))
        )

    def module_virtual(self, t, n, r, l, k):
        command = ""

        if k is None:
            k = 0

        if r:
            command = "./bin/virtual_esp -t %s -n %s -l %s -k %s -r" % (
                t,
                n,
                l,
                k,
            )
        else:
            command = "./bin/virtual_esp -t %s -n %s -l %s -k %s " % (
                t,
                n,
                l,
                k,
            )

        self.run_command_in_docker(
            "cd library"
            + "&&"
            + "cmake ."
            + "&&"
            + "make virtual_esp"
            + "&&"
            + "echo '\n\033[95m===============================================================================\nRunning the virtual network:\033[0m'"
            + "&&"
            + "echo '\033[95m>> The colorful outputs are outputted by the virtual network\033[0m'"
            + "&&"
            + "echo '\033[95m>> The white outputs are outputted by ramen\033[0m\n'"
            + "&&"
            + command
            + "&&"
            # Delete gcov files related to the simulator, we don't want them
            + "gcovr --delete --root='.' --filter='virtual_esp.*' > /dev/null 2>&1"
        )

    def module_test(self):
        self.run_command_in_docker(
            "cd library"
            + "&&"
            + "cmake ."
            + "&&"
            + "make ramen_unit_tests"
            + "&&"
            + "echo '\n\033[96m===============================================================================\nRunning the tests:\033[0m\n'"
            + "&&"
            + "./bin/ramen_unit_tests"
            + "&&"
            + "gcovr --print-summary --root='.' --filter='/ramen/library/src/ramen/*' --exclude='virtual_esp.*'"
            + "&&"
            + "echo '\n\033[96mGenerating the HTML version of the coverage report under docs/coverage-report\033[0m'"
            + "&&"
            + "gcovr --delete --root='.' --filter='/ramen/library/src/ramen/*' --exclude='virtual_esp.*' --html --html-details -o ../docs/coverage-report/coverage.html"
        )

    def module_init(self):
        self.check_root_access()

        print(
            self.color["OKGREEN"]
            + ">> Installing required system packages"
            + self.color["ENDC"]
        )
        os.system("apt install -y -qq docker clang clang-format clang-tidy")

        print(
            self.color["OKGREEN"]
            + ">> Installing required python packages"
            + self.color["ENDC"]
        )
        os.system("pip3 install -U platformio black pylint")

        print(
            self.color["OKGREEN"]
            + ">> Initialing the submodules"
            + self.color["ENDC"]
        )
        os.system("git submodule update --init --recursive")

        print(
            self.color["OKGREEN"]
            + ">> Checking out the correct version of the submodules"
            + self.color["ENDC"]
        )
        os.system(
            "cd library/test/TaskScheduler"
            + "&&"
            + "git checkout b36cc818db89430b7564d1c56a668937f6dae6ec"
        )

        print(self.color["OKGREEN"] + ">> DONE <<" + self.color["ENDC"])

    def run_command_in_docker(self, command, privileged=False):
        """
        Runs the given command in the docker container
        """

        if privileged:
            privileged_option = "--privileged"
        else:
            privileged_option = ""

        self.check_root_access()

        os.system(
            'docker run --user=1000 %s -t -v %s:/ramen -v %s:/.platformio ghcr.io/a5-015/ramen/ramen-dev /bin/bash -c "%s"'
            % (
                privileged_option,
                self.project_path,
                os.path.join(self.project_path, ".cache"),
                command,
            )
        )

    def check_root_access(self):
        """
        Checks if the script has root privileges
        """

        if os.geteuid() != 0:
            print("Please run me with sudo or give me root access somehow")
            self.exit_code()

    def check_submodules(self):
        """
        Checks if the submodules are initialized
        """

        for module in self.submodules:
            if (
                os.listdir(
                    os.path.join(self.project_path, "library/test", module)
                )
                == []
            ):
                print(
                    "Initialize submodules first with 'git submodule update --init --recursive'"
                )
                self.exit_code()

    def chown_ttyUSBX_to_user(self, port="0"):
        """
        Applies chown 1000:dialout to /dev/ttyUSB0 in order to upload code to the board
        """

        self.check_root_access()
        print(
            self.color["OKCYAN"]
            + ">> 'sudo chown 1000:dialout /dev/ttyUSB%s'" % port
            + self.color["ENDC"]
        )
        os.system("chown 1000:dialout /dev/ttyUSB%s" % port)

    def exit_code(self):
        exit()


if __name__ == "__main__":
    args = parser.parse_args()
    run_dev = RunDev(args)
