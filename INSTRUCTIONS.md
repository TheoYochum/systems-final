## Instructions
 - The makefile has 5 different targets, run `make compile` once
 - All variations on the algorithm prompt for the number of iterations and length of an array, as due to being a fundamentally random algorithm the timing of a single run is not a valid depiction. It then runs the algorithm and prints out some statistics, each prints out the current iteration it is on as a form of progress tracker.
 - `make single` runs a single threaded bogosort
 - `make local` runs a multithreaded bogosort using 12 subprocesses which should require 6 cores to see full utilization.
 - `make client` is the user end of the distributed network and will attempt to connect to all ip addresses from `149.89.161.101` to `149.89.161.134` which will be valid servers if they have been setup. It is assumed that this is run from `149.89.161.100` however it may work from other addresses.
 - `make server` establishes a server on the current computer and awaits a client to attempt to connect to it. This end processes the information and will send back to the client. If it is run from a terminal instance it prints tracking information.
 - `server.sh` is a bash script that attempts to connect to all CS lab computers and establish a server on them, intended to be followed by `make client`.

 ## Current Features
 - The ability to run and time bogosort in a single threaded, multithreaded, and distributed algorithm.
 - Robust interprocess communication
 - A script to automate the establishment of servers

 ## Current Bugs
 - The distributed algorithm appears to run too fast on smaller arrays, getting out of sync and crashing. Keep it at `n >= 11`.
 - As of testing on 1/18 `149.89.161.101` and `149.89.161.102` do not establish a connection potentially due to them being used for testing of the bash script, `149.89.161.115` also does not connect but appears to simply be turned off with a different error. **Both 101 and 102 work after switching the port however the same issue may arise, but seemed to come from ssh testing not the actual program**
 - All servers should be closed after proper use however if left open they have undefined behavior 