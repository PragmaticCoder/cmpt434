#!/bin/bash
# This script is used to automate debugging

# Setting Default
PROGRAM="client_v1"
IP="0"
PORT="34821"

usage() {
  echo "Usage ${0} [-r][-d][-v] <client|server>" >&2
  echo 'Debug with automated tools'
  echo '-d Runs in debug mode with GDB.'
  echo '-v Runs in debug mode with Valgrind.'
  exit 1
}

debug_gdb() {
  echo 'Debugging with GDB'
  if [[ "${PROGRAM}" = "server_v1" ]]; then
    gdb -x .gdbinit_server ./bin/$1
  elif [[ "${PROGRAM}" = "client_v1" ]]; then
    gdb -x .gdbinit_client ./bin/$1
  fi
  exit 0
}

debug_valgrind() {
  echo 'Debugging with Valgrind'
  if [[ "${PROGRAM}" = "server_v1" ]]; then
    valgrind ./bin/$1 $PORT
  elif [[ "${PROGRAM}" = "client_v1" ]]; then
    valgrind ./bin/$1 $IP $PORT
  fi

  exit 0
}

select_program() {
  if [[ "${OPTARG}" = "server" ]]; then
    PROGRAM="server_v1"
  elif [[ "${OPTARG}" = "client" ]]; then
    PROGRAM="client_v1"
  fi
}

echo 'Running Sanity Test'

while getopts "mr:d:v:" OPTION; do
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
  m)
    echo "Rebuild using Make"
    make clean && clear && make
    ;;
  ?)
    usage
    ;;
  esac
done

if [[ "${PROGRAM}" = "server_v1" ]]; then
  ./bin/$PROGRAM $PORT
elif [[ "${PROGRAM}" = "client_v1" ]]; then
  ./bin/$PROGRAM $IP $PORT
fi

exit 0
