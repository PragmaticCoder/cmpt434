#!/bin/bash
# This script is used to automate debugging

# Setting Default
PROGRAM="client_v1"

usage() {
  echo "Usage ${0} [-r][-d][-v] <client|server>" >&2
  echo 'Debug with automated tools'
  echo '-d Runs in debug mode with GDB.'
  echo '-v Runs in debug mode with Valgrind.'
  exit 1
}

debug_gdb() {
  echo 'Debugging with GDB'
  gdb -x .gdbinit ./bin/$1
  exit 0
}

debug_valgrind() {
  echo 'Debugging with Valgrind'
  valgrind ./bin/$1 0.0.0.0 30000
  exit 0
}

select_program() {
  if [[ "${OPTARG}" = "server" ]]; then
    PROGRAM="server_v1"
  elif [[ "${OPTARG}" = "client" ]]; then
    PROGRAM="client_v1"
  fi
}

make clean && clear && make

echo 'Running Sanity Test'

while getopts "r:d:v:" OPTION; do
  case ${OPTION} in
  r)
    echo "selected - RUN MODE"
    select_program
    ;;
  d)
    echo "GDB DEBUG MODE"
    select_program
    debug_gdb $PROGRAM
    ;;
  v)
    echo "VALGRIND DEBUG MODE"
    select_program
    debug_valgrind $PROGRAM
    ;;
  ?)
    usage
    ;;
  esac
done

./bin/$PROGRAM 0.0.0.0 30000

exit 0
