
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
To launch part A server and client program,
- Go to asn_01 directory
- We have automated the build process so just try:
  In one terminal:
  ./sanity_test.sh -r server_v1 33000
  In second terminal:
 ./sanity_test.sh -r client_v1 127.0.0.1  33000


"
 and for arguments: its just "client" and "server"

# Question 2
- To Launch Proxy server:
  make all
  In terminal 1:
  ./bin/server <server-port>

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