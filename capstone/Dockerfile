# Download base image ubuntu 16.04
FROM ubuntu:16.04

# Install required packages
RUN apt update -qq && \ 
    apt install -y -qq python-pip git wget cmake libboost-system-dev && \
    pip install platformio && \
    platformio lib --global install painlessMesh

# Clean the packages
RUN apt remove -y -qq git wget && \
    apt autoremove -y -qq && \
    rm -rf /var/lib/apt/lists/*

# Set /ramen as the default working directory
WORKDIR /ramen