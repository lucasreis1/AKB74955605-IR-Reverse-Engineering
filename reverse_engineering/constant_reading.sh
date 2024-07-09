#!/bin/bash

# Read codes constantly from ir-ctl and convert them to binary 
# until interrupted by SIGINT

trap interrupt_sig INT

interrupt_sig() {
  echo "Captured SIGINT. Exiting..."
  exit 0
}

while true
do
  ir-ctl -d /dev/lirc1 -r -1 | ./convert_to_bin.sh
  echo 
done
