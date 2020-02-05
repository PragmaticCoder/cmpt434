# Instructions

```text
  Name: Alvi Akbar
  NSID: ala273
  Student No: 11118887
```

We use the run.sh script to run all commands:
If `run.sh` file is not executable, please run:
`chmod +x run.sh` first.

Incase automated testing suite is not running, please run `chmod +x runtests.sh`

```text
 -m flag is used for making the project.
 -r flag is run operation
 -d gdb with layout next
 -v valgrind inspection
```

## Question 1

  **Terminal 1:**
  `./run.sh -mr server_v1 <server port>`
  **Terminal 2:**
  `./run.sh -r client_v1 <server-ip>  <server port>`

  Type `add` `getvalue` and `getall` from client terminal as specified in assignment description.

## Question 2

`make all`
  **Terminal 1:**
  `./run.sh -mr server_v2 <port 1>`
  **Terminal 2:**
  `./run.sh -r proxy_v2 <server ip> <port 1> <port 2>`
  **Terminal 3:**
  `./run.sh -r client_v2 <server ip> <port 2>`

  Type `add` `getvalue` and `getall` from client terminal as specified in assignment description.

## Question 3

`make all`
  **Terminal 1:**
  `./run.sh -mr server_v2 <port 1>`
  **Terminal 2:**
  `./run.sh -r server_v3 <port 1>`
  **Terminal 3:**
  `./run.sh -r proxy_v3 <server ip> <port 1> <port 2>`
  **Terminal 4:**
  `./run.sh -r client_v3 <server ip> <port 3>`

## Troubleshooting Steps

If for any reason, project built fails and we need to build it manually, please make sure to run `export LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH`
after making. This is needed to link the static library.