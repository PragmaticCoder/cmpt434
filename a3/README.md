# Instructions

```text
  Name: Alvi Akbar
  NSID: ala273
  Student No: 11118887
```

We use the run.sh script to run our program:
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

`make clean && make all`

## Usage

`./run.sh -r name host_port connect_port1 [connect_port2]`

## Example Program

Following commands are to be **executed** in **sequence** to construct the router network as shown in the figure below:

**Terminal 1**
`./run.sh -mr A 40000 40001`

**Terminal 2**
`./run.sh -r B 40001 40002 40004`

**Terminal 3**
`./run.sh -r C 40002 40001 40003`

**Terminal 4**
`./run.sh -r D 40003 40002 40004`

**Terminal 5**
`./run.sh -r E 40004 40001 40003`

**Router Diagram**
```
 +-------+         +-------+        +-------+
 |       |   1     |       |   1    |       |
 |   E   |---------|   B   |--------|   A   |
 |       |         |       |        |       |
 +-------+         +-------+        +-------+
     |                 |
    1|                 |1
     |                 |
 +-------+         +-------+
 |       |         |       |
 |   D   |---------|   C   |
 |       |    1    |       |
 +-------+         +-------+
```

Refer to `documentation.txt` about more info about calculation for the routing table constructor above

## Troubleshooting Steps

* If for any reason, generating built fails in test phase, please take out the `tests`
from Makefile:36 `all: build $(TARGET) $(PROGRAMS) tests` before executing the `make` command.

* Check `run.sh` file is not executable file. If `run.sh` file is not executable, please run: `chmod +x run.sh` before executing `make`

* Incase automated testing suite is not running, please run `chmod +x tests/runtests.sh` command before executing `make`
  