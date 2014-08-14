#!/bin/bash
CUR_DIR=`pwd`
PREFIX=/usr/local
if [ $# -ge 1 ]; then
    # The user supplied an argument
    PREFIX=${1}
    # Get absolute path name of install directory
    mkdir -p "${PREFIX}" 2> /dev/null
    cd "${PREFIX}" > /dev/null 2>&1
    if [ $? != 0 ] ; then
        echo "ERROR: '${PREFIX}' does not exist nor could be created."
        echo "Please choose another directory."
        exit 1
    else
        PREFIX=`pwd -P`
    fi
fi
echo "cluster-delegator will be installed in '${PREFIX}'"
cd ${CUR_DIR}
if [ ! -d ${PREFIX}/include ]; then
    mkdir ${PREFIX}/include
fi
cp -rf ./include/delegator ${PREFIX}/include/delegator
