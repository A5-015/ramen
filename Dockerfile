# Download base image ubuntu 16.04
FROM ubuntu:16.04

# Install required packages
RUN apt update -qq && \ 
    apt install -y -qq software-properties-common build-essential && \
    add-apt-repository ppa:deadsnakes/ppa &&\
    apt update -qq && \
    apt install -y -qq python3.7 python3-pip git wget cmake libboost-system-dev doxygen graphviz && \
    easy_install3 pip && \
    python3.7 -m pip install platformio

# Clean the packages
RUN apt remove -y -qq git wget python python3.5 && \
    apt autoremove -y -qq && \
    rm -rf /var/lib/apt/lists/*

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

# Set /ramen as the default working directory
WORKDIR /ramen