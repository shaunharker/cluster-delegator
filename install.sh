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
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SHELL_DIR/.install/parse.sh

echo "cluster-delegator will be installed in '${PREFIX}'"

CUR_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd ${CUR_DIR}
if [ ! -d ${PREFIX}/include ]; then
    mkdir ${PREFIX}/include
fi
echo Copying ./include/delegator to ${PREFIX}/include/delegator
if [ ! -d ${PREFIX}/include/delegator ]; then
    mkdir ${PREFIX}/include/delegator
fi
cp -rfv ./include/delegator/* ${PREFIX}/include/delegator
