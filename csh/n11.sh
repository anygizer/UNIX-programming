#!/bin/csh -f

set usage = "Usage:     $0 [options] source_sirectory target_directory"
set v = ""

while ("$1" =~ -*)
    switch("$1")
	case -v:
	    set v = 1
	    shift
	    breaksw
	case -d:
	    set d = 1
	    shift
	    breaksw
	case -help:
	default:
	    echo "Copies source directory subdirectories to the target directory."
	    echo "$usage"
	    echo "           -v    verbose output of directores being copied"
	    echo "           -d    clear target_directory before copying"
	    exit 1
    endsw
end

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

if ( $d == 1 && `ls -a $2 | wc | awk '{print $1}'` != 2) then
    cd "$2"
    rm -fr *          #rm -fri *
    cd "$startwd"
endif

cd "$1"

foreach directory  (`find . -type d`)
    if ( $v == 1) echo "${directory}"
    mkdir -p "${startwd}/$2/${directory}"
end

cd "$startwd"
