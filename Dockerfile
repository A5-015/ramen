# Download base image ubuntu 16.04
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive 

# Install required packages
RUN apt update -qq && \ 
    apt install -y software-properties-common build-essential python3-pip cmake doxygen graphviz lcov && \
    pip3 install platformio

# Clean the packages
RUN apt autoremove -y -qq && \
    rm -rf /var/lib/apt/lists/*

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

# Set /ramen as the default working directory
WORKDIR /ramen