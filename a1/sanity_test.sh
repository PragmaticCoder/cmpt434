#!/bin/bash
# This script is used to automate debugging

# Setting Default

export LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH

usage() {
  echo "Usage ${0} [-r][-d][-v] <client|server>" >&2
  echo 'Debug with automated tools'
  echo '-d Runs in debug mode with GDB.'
  echo '-v Runs in debug mode with Valgrind.'
  exit 1
}

debug_gdb() {
  echo 'Debugging with GDB'
  if [[ "${1}" = "server_v1" ]]; then
    gdb --args ./bin/$1 $2
  elif [[ "${1}" = "client_v1" ]]; then
    gdb --args ./bin/$1 $2 $3
  elif [[ "${1}" = "server_v2" ]]; then
    gdb --args ./bin/$1 $2
  elif [[ "${1}" = "client_v2" ]]; then
    gdb --args ./bin/$1 $2 $3
  elif [[ "${1}" = "tcp_tcp_proxy" ]]; then
    gdb --args ./bin/$1 $2 $3 $4
  elif [[ "${1}" = "client_v3" ]]; then
    gdb --args ./bin/$1 $2 $3
  elif [[ "${1}" = "server_v3" ]]; then
    gdb --args ./bin/$1 $2
  elif [[ "${1}" = "udp_tcp_proxy" ]]; then
    gdb --args ./bin/$1 $2 $3 $4
  fi
  exit 0
}

debug_valgrind() {
  echo 'Debugging with Valgrind'
  if [[ "${1}" = "server_v1" ]]; then
    valgrind ./bin/$1 $2
  elif [[ "${1}" = "client_v1" ]]; then
    valgrind ./bin/$1 $2 $3
  elif [[ "${1}" = "server_v2" ]]; then
    valgrind ./bin/$1 $2
  elif [[ "${1}" = "client_v2" ]]; then
    valgrind ./bin/$1 $2 $3
  elif [[ "${1}" = "tcp_tcp_proxy" ]]; then
    valgrind ./bin/$1 $2 $3 $4
  elif [[ "${1}" = "client_v3" ]]; then
    valgrind ./bin/$1 $2 $3
  elif [[ "${1}" = "server_v3" ]]; then
    valgrind ./bin/$1 $2
  elif [[ "${1}" = "udp_tcp_proxy" ]]; then
    valgrind ./bin/$1 $2 $3 $4
  fi

  exit 0
}

select_program() {
  PROGRAM=""
  if [[ "${OPTARG}" = "server_v1" ]]; then
    PROGRAM="server_v1 "$1
  elif [[ "${OPTARG}" = "client_v1" ]]; then
    PROGRAM="client_v1 "$1" "$2
  elif [[ "${OPTARG}" = "server_v2" ]]; then
    PROGRAM="server_v2 "$1
  elif [[ "${OPTARG}" = "client_v2" ]]; then
    PROGRAM="client_v2 "$1" "$2
  elif [[ "${OPTARG}" = "proxy_v2" ]]; then
    PROGRAM="tcp_tcp_proxy "$1" "$2" "$3
  elif [[ "${OPTARG}" = "server_v3" ]]; then
    PROGRAM="server_v3 "$1
  elif [[ "${OPTARG}" = "client_v3" ]]; then
    PROGRAM="client_v3 "$1" "$2
  elif [[ "${OPTARG}" = "proxy_v3" ]]; then
    PROGRAM="udp_tcp_proxy "$1" "$2" "$3
  fi
}

echo 'Running Sanity Test'

while getopts "mr:d:v:" OPTION; do
  case ${OPTION} in
  r)
    echo "selected - RUN MODE"
    select_program $3 $4 $5
    echo "running program: ${PROGRAM}"

    if [[ "$PROGRAM" = "" ]]; then
      echo "Cant able to find the execuetable please verify"
      exit 0
    else
      ./bin/${PROGRAM}
    fi
    ;;
  d)
    echo "GDB DEBUG MODE"
    select_program $3 $4 $5
    if [[ "$PROGRAM" = "" ]]; then
      echo "Cant able to find the execuetable please verify"
      exit 0
    else
      debug_gdb ${PROGRAM}
    fi
    ;;
  v)
    echo "VALGRIND DEBUG MODE"
    select_program $3 $4 $5
    if [[ "$PROGRAM" = "" ]]; then
      echo "Cant able to find the execuetable please verify"
      exit 0
    else
      debug_valgrind ${PROGRAM}
    fi
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

exit 0
