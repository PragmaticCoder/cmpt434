#!/bin/bash
# This script is used to automate debugging

# Setting Default
PROGRAM="client_v1"
IP="0"
PORT="33333"
DPORT="0"

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
    gdb -x gdb ./bin/$1
  exit 0
}

debug_valgrind() {
  echo 'Debugging with Valgrind'
  if [[ "${PROGRAM}" = "server_v1" ]]; then
    valgrind ./bin/$1 $PORT
  elif [[ "${PROGRAM}" = "client_v1" ]]; then
    valgrind ./bin/$1 $IP $PORT
  elif [[ "${PROGRAM}" = "server_v2" ]]; then
    valgrind ./bin/$1 $PORT
  elif [[ "${PROGRAM}" = "client_v2" ]]; then
    valgrind ./bin/$1 $IP $PORT
  elif [[ "${PROGRAM}" = "tcp_tcp_proxy" ]]; then
    valgrind ./bin/$1 $IP $DPORT $PORT
  elif [[ "${PROGRAM}" = "client_v3" ]]; then
    valgrind ./bin/$1 $IP $PORT
  elif [[ "${PROGRAM}" = "server_v3" ]]; then
    valgrind ./bin/$1 $PORT
  elif [[ "${PROGRAM}" = "udp_tcp_proxy" ]]; then
    valgrind ./bin/$1 $IP $DPORT $PORT
  fi

  exit 0
}

select_program() {
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
	IP 	= $3
	PORT 	= $4
	DPORT 	= $5
    select_program $3 $4 $5
    ./bin/${PROGRAM}
    ;;
  d)
    echo "GDB DEBUG MODE"
    select_program $3 $4 $5
    debug_gdb $PROGRAM
    ;;
  v)
    echo "VALGRIND DEBUG MODE"
    select_program $3 $4 $5
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
