# Instructions

```text
  Name: Alvi Akbar
  NSID: ala273
  Student No: 11118887
```

We use the run.sh script to run all commands:
If `run.sh` file is not executable, please run:
`chmod +x run.sh` first.

Incase automated testing suite is not running, please run `chmod +x tests/runtests.sh`

```text
 -m flag is used for making the project.
 -r flag is run operation
 -d gdb with layout next
 -v valgrind inspection
```

## Make

`make all`

## Client

`./run.sh -r client <localhost/hostname> <port-no> <window-size> <timeout(s)>`

## Server

`./run.sh -r server <port-no> <probability>`

## Forwarder

`./run.sh -r forwarder <localhost/hostname> <dest-port> <window-size> <timeout> <src-port> <probability>`

## Troubleshooting Steps

If for any reason, project built fails in test phase, please take out the `tests`
from Makefile:36 `all: build $(TARGET) $(PROGRAMS) tests`before running the make
command.
