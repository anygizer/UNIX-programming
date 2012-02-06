#!/bin/bash

usage="usage: $0 source_sirectory target_directory"

error_exit()
{
  echo "$1" 1>&2
  exit 1
}

# check if the both directories are specified
if [ -z $1 ] || [ -z $2 ]; then
echo $usage
  exit
fi

# check if the first parameted is a directory
if [ ! -d $1 ]; then
error_exit "$1: Not a directory"
fi

# check if the second parameter is a directory
if [ ! -d $2 ]; then
error_exit "$2: Not a directory"
fi

startpwd=$PWD

cd "$1"

for directory in $(find . -type d); do
#    echo "${directory}"
    mkdir -p "${startpwd}/$2/${directory}"
done

cd "$startpwd"