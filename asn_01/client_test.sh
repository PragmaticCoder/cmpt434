#!/bin/bash
# Simple tcp cl using netcat
#  - depending on the netcat version either use nc -l 30000 or nc -l -p 30000
#  - verify with `telnet localhost 30000`
#  - quit the telnet with `ctrl-]` and then type quit
#  - the while loop is there so reopen the port after a client has disconnected
#  - supports only one client at a time
PORT=30000;
while :; do ./bin/client_v1 0.0.0.0 30000 | tee  output.log; sleep 1; done
