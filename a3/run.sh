#!/bin/bash

# This script is used to automate debugging

# Setting Default

usage() {
  echo "Usage ${0} [-r][-d][-v] <name> <port> <destination port> [[opt port]..]" >&2
  echo 'Debug with automated tools'
  echo '-d Runs in debug mode with GDB.'
  echo '-v Runs in debug mode with Valgrind.'
  exit 1
}

echo 'Running Sanity Test'

while getopts "mr:d:v:" OPTION; do
  shift $((OPTIND - 2))
  case ${OPTION} in
  r)
    echo "selected - RUN MODE"		
	./build/Server $@
    ;;
  d)
    echo "GDB DEBUG MODE"
	gdb --arg ./build/Server $@
    ;;
  v)
    echo "VALGRIND DEBUG MODE"
    	valgrind ./build/Server $@
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

