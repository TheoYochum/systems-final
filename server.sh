#!/bin/bash
USERNAME=tyochum30
HOSTS="01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34"
# SCRIPT="pwd"
SCRIPT="cd Documents/Systems/Labs/project-iiits-the-final-project-4-yochum-divideandconquer/; nohup make server > /dev/null 2>&1 &"
for HOSTNAME in ${HOSTS} ; do
    ssh -l ${USERNAME} 149.89.161.1${HOSTNAME} "${SCRIPT}"
done
