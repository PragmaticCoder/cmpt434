
# Instructions

We use the run.sh script to run all commands:
If `run.sh` file is not executable, please run:
`chmod +x run.sh` first.

```bash
 -m flag is used for making the project.
 -r flag is run operation
 -d gdb with layout next
 -v valgrind inspection
```

# Question 1

  **Terminal 1:**
  `./run.sh -mr server_v1 <server port>`
  **Terminal 2:**
  `./run.sh -r client_v1 <server-ip>  <server port>`

  Type `add` `getvalue` and `getall` from client terminal as specified in assignment description.

# Question 2

  **Terminal 1:**
  `./run.sh -mr server_v2 <server port>`
  **Terminal 2:**
  `./run.sh -r tcp_tcp_proxy <server ip>  <server port> <client port>`
  **Terminal 3:**
  `./bin/client_v1 <server port>`

  Type `add` `getvalue` and `getall` from client terminal as specified in assignment description.

# Question 3

make all
  In terminal 1:
  ./bin/client_v1 30000

  In terminal 2:
  ./bin/server_v1 40000

  In terminal 3:
  ./bin/tcp_tcp_proxy 127.0.0.1 40000 30000

  In terminal 4:
  ./bin/udp_server 40000