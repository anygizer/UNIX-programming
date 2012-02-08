#!/bin/csh -f

set usage = "Usage:     $0 [options] source_sirectory target_directory"
set v = 0
set d = 0

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

find . -type d -exec csh -c 'mkdir -p -m `stat -c "%a" "{}"` "$1" ; chown `stat -c "%U:%G" "{}"` "$1"' "${startwd}/$2/{}" \;

cd "$startwd"
