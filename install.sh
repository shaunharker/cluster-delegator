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

## Build the library
./.install/build.sh --prefix=$PREFIX --searchpath=$SEARCHPATH --build=$BUILDTYPE --test $MASS || exit 1

## Install amalgamated header file
## For use with #include <cluster-delegator.hpp>
cd ${SRC_DIR}
./.install/amalgamate.sh || exit 1
mkdir -p ${PREFIX}/include/delegator
cp ./dist/include/cluster-delegator.hpp ${PREFIX}/include/cluster-delegator.hpp
cp ./dist/include/cluster-delegator.h   ${PREFIX}/include/cluster-delegator.h
cp ./dist/include/cluster-delegator.hpp ${PREFIX}/include/delegator/delegator.h

## Tell the user about it
echo "cluster-delegator has been installed in '${PREFIX}'"
ls -l ${PREFIX}/lib/libcluster-delegator*
ls -l ${PREFIX}/include/cluster-delegator*
ls -l ${PREFIX}/include/delegator/delegator.h 

echo " Note: The following two files are the same   "
echo " --> ${PREFIX}/include/cluster-delegator.hpp  "
echo " --> ${PREFIX}/include/delegator/delegator.h  "
echo " (The latter is deprecated but remains for backwards compatibility;  "
echo "  please use #include <cluster-delegator.hpp> instead)"
