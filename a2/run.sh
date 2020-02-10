#!/bin/bash

# This script is used to automate debugging

# Setting Default

usage() {
  echo "Usage ${0} [-r][-d][-v] <client|server>" >&2
  echo 'Debug with automated tools'
  echo '-d Runs in debug mode with GDB.'
  echo '-v Runs in debug mode with Valgrind.'
  exit 1
}

debug_gdb() {
  echo 'Debugging with GDB'
  if [[ "${1}" = "server" ]]; then
    gdb --args ./build/$1 $2 $3
  elif [[ "${1}" = "client" ]]; then
    gdb --args ./build/$1 $2 $3 $4 $5
  elif [[ "${1}" = "forwarder" ]]; then
    gdb --args ./build/$1 $2 $3 $4 $5 $6 $7
  fi
  exit 0
}

debug_valgrind() {
  echo 'Debugging with Valgrind'
  if [[ "${1}" = "server" ]]; then
    valgrind ./build/$1 $2 $3
  elif [[ "${1}" = "client" ]]; then
    valgrind ./build/$1 $2 $3 $4 $5
  elif [[ "${1}" = "forwarder" ]]; then
    valgrind ./build/$1 $2 $3 $4 $5 $6 $7
  fi

  exit 0
}

select_program() {
  PROGRAM=""
  if [[ "${OPTARG}" = "server" ]]; then
    PROGRAM="server "$1" "$2
  elif [[ "${OPTARG}" = "client" ]]; then
    PROGRAM="client "$1" "$2" "$3" "$4
  elif [[ "${OPTARG}" = "forwarder" ]]; then
    PROGRAM="forwarder "$1" "$2" "$3" "$4" "$5" "$6
  fi
}

echo 'Running Sanity Test'

while getopts "mr:d:v:" OPTION; do
  case ${OPTION} in
  r)
    echo "selected - RUN MODE"
    select_program $3 $4 $5 $6 $7 $8
    if [[ "$PROGRAM" = "" ]]; then
      echo "Cant able to find the execuetable please verify"
      exit 0
    else
      ./build/${PROGRAM}
    fi
    ;;
  d)
    echo "GDB DEBUG MODE"
    select_program $3 $4 $5 $6 $7 $8
    if [[ "$PROGRAM" = "" ]]; then
      echo "Cant able to find the execuetable please verify"
      exit 0
    else
      debug_gdb ${PROGRAM}
    fi
    ;;
  v)
    echo "VALGRIND DEBUG MODE"
    select_program $3 $4 $5 $6 $7 $8
    if [[ "$PROGRAM" = "" ]]; then
      echo "Cant able to find the execuetable please verify"
      exit 0
    else
      debug_valgrind ${PROGRAM}
    fi
    ;;
  m)
    echo "Rebuild using Make"
    make clean && make && clear
    ;;
  ?)
    usage
    ;;
  esac
done

exit 0
