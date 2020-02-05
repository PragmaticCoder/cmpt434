
# Instructions

We use the run.sh script to run all commands:
If `run.sh` file is not executable, please run:
`chmod +x run.sh` first.

```
 -m flag is used for making the project.
 -r flag is run operation
 -d gdb with layout next
 -v valgrind inspection
```

# Question 1

  **Terminal 1:**
  `./run.sh -mr server_v1 33000`
  **Terminal 2:**
  `./run.sh -r client_v1 127.0.0.1  33000`

# Question 2

- To Launch Proxy server:
  `make all`

  **In terminal 1**:
  `./run.sh -r server-port`

  In terminal 2:
  ./bin/tcp_tcp_proxy 127.0.0.1 <server-port> <client-port>

  In terminal 3:
  ./bin/client_v1 <server-port>
  and then start typing the tcpip comments from client terminal

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