#!/bin/csh -f

set usage="Usage:     $0 source_sirectory target_directory"

# check if the both directories are specified
if ( $#argv < 2) then
    echo "$usage"
    exit 1
endif

# check if the first parameted is a directory
if ( ! -d "$1" ) then
    echo "${1}: Not a directory"
    exit 1
endif

# check if the second parameter is a directory
if ( ! -d "$2" ) then
    echo "${2}: Not a directory"
    exit 1
endif

set startwd="$cwd"

cd "$1"

foreach directory  (`find . -type d`)
#    echo "${directory}"
    mkdir -p "${startwd}/$2/${directory}"
end

cd "$startwd"
