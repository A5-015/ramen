# Download base image ubuntu 16.04
FROM ubuntu:16.04

# Update Ubuntu Software repository
RUN apt update

# Add time zone data
RUN DEBIAN_FRONTEND=noninteractive && \
    apt install tzdata

# Install required linux packages
RUN apt install -y ocaml ocaml-native-compilers camlp4-extra opam make m4 git nodejs npm pkg-config

# Initialize opam and install required ocaml packages
RUN export OPAMYES=1 && \
    opam init && \
    eval `opam config env` && \
    opam depext conf-pkg-config.1.1

# Install required ocaml packages
RUN opam install cstruct ipaddr lwt.2.4.8 cstruct.3.0.2 sexplib cmdliner.0.9.7 yojson.1.3.0 core.113.00.00 cstruct-lwt.3.0.2

# cd into home
WORKDIR /home

# Clone the repositories
RUN git clone https://github.com/consensus-oracle/coracle && \
    git clone https://github.com/heidi-ann/ocaml-raft-data && \
    git clone https://github.com/heidi-ann/ocaml-raft

# Build ocaml-raft
RUN eval `opam config env` && \
    cd ocaml-raft && \
    make

# Build Coracle
RUN eval `opam config env` && \
    cd coracle && \
    ocaml setup.ml -clean && \
    ocaml setup.ml -configure && \
    ocaml setup.ml -build && \
    rm /root/.opam/system/lib/unix/META && \
    ocaml setup.ml -install

# Clean the packages
RUN apt remove -y git && \
    apt autoremove -y && \
    rm -rf /var/lib/apt/lists/*

# Example ways to run each simulator
# RUN eval `opam config env` && \
#     ./ocaml-raft/cmdstart.byte -candidate "Uniform-11-22" -follower "Uniform-150-155" -leader "Fixed-75" -nodes 3 -delay "Fixed-7"
# RUN eval `opam config env` && \
#     ./coracle/coracle_sim.byte
