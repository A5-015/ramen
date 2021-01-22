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


if args.target == "test":
    check_image()

    os.system(
        "docker run --user=1000 -v %s:/ramen ramen-dev /bin/bash -c \"cmake . -DCMAKE_CXX_FLAGS='-Wall -Werror' && make && run-parts --regex catch_ bin/\""
        % project_path
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
