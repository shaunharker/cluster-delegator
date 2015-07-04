#!/bin/bash
#  Usage: install.sh [--prefix=INSTALLATION_PREFIX]
#  Effects: Copies header files into INSTALLATION_PREFIX/include
#           If no prefix is provided, it chooses 
#            /usr/local 
#            unless it cannot write there, in which case it chooses
#            ~/.local 
#            unless it does not exist, in which case it fails.

## Parse command line arguments to get
#  PREFIX, SEARCHPATH, BUILDTYPE, TESTS, and MASS
SRC_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SRC_DIR/.install/parse.sh

## Install amalgamated header file
## For use with #include <cluster-delegator.hpp>
cd ${SRC_DIR}
./.install/amalgamate.sh
mkdir -p ${PREFIX}/include/delegator
cp ./dist/include/cluster-delegator.hpp ${PREFIX}/include/cluster-delegator.hpp
cp ./dist/include/cluster-delegator.hpp ${PREFIX}/include/delegator/delegator.h

## Tell the user about it
echo "cluster-delegator has been installed in '${PREFIX}'"
echo " --> ${PREFIX}/include/cluster-delegator.hpp "
echo " --> ${PREFIX}/include/delegator/delegator.h "
echo " (These files are the same; the latter is for"
echo "  backwards compatibility.)"
