#!/bin/csh -f

set usage = "$0 [OPTION] extension directory"
set v = ''

#while first argument starts with dash
while ("$1" =~ -*)
    switch("$1")
	case -v:
	    set v = '-v'
	    shift
	    breaksw
	case --help:
	case -h:
	default:
	    echo "Removes files with the specified extension under the specified directory recursively."
	    echo "Usage:"
	    echo "$usage"
	    echo "           -v     verbose output of files being removed"
	    echo "           -h, --help"
	    echo "                  output this message"
	    exit 1
    endsw
end

# check if the both arguments are specified
if ( $#argv < 2) then
    echo "$usage"
    exit 1
endif

# check if the second parameter is a directory
if ( ! -d "$2" ) then
    echo "${2}: Not a directory"
    exit 1
endif

set dollar='$'
set regex=".*\.${1}$dollar"
find $2 -type f -regex "$regex" -exec rm $v {} \;
