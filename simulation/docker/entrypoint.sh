#!/usr/bin/env bash


trap 'kill -TERM $PID' TERM INT
python base64_to_json.py $1 > network.json
./coracle/coracle_sim.byte -f network.json && echo & 
PID=$!
wait $PID
trap - TERM INT
wait $PID
EXIT_STATUS=$?