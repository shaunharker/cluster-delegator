#!/bin/bash

SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR/..
mkdir -p dist/include
OUTFILE=./dist/include/cluster-delegator.hpp


echo '/// cluster-delegator.hpp (amalgated header)' > $OUTFILE
echo >> $OUTFILE && echo '/*' >> $OUTFILE
echo >> $OUTFILE && cat LICENSE >> $OUTFILE
echo >> $OUTFILE && echo '*/' >> $OUTFILE
echo >> $OUTFILE && echo '#ifndef CLUSTERDELEGATOR_HPP_INCLUDED' >> $OUTFILE
echo '#define CLUSTERDELEGATOR_HPP_INCLUDED' >> $OUTFILE
echo '#define CLUSTER_DELEGATOR_IS_AMALGAMATION' >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Message.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Message.hpp >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Communicator.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Communicator.hpp >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Coordinator_Worker_Process.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Coordinator_Worker_Process.hpp >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Coordinator_Worker_Scheme.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Coordinator_Worker_Scheme.hpp >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/delegator.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/delegator.hpp >> $OUTFILE
echo >> $OUTFILE && echo '#endif' >> $OUTFILE

OUTFILE=./dist/include/cluster-delegator.h
echo >> $OUTFILE && echo '/// cluster-delegator.h (amalgated header)' > $OUTFILE
echo >> $OUTFILE && echo '/*' >> $OUTFILE
echo >> $OUTFILE && cat LICENSE >> $OUTFILE
echo >> $OUTFILE && echo '*/' >> $OUTFILE
echo >> $OUTFILE && echo '#ifndef CLUSTERDELEGATOR_H_INCLUDED' >> $OUTFILE
echo '#define CLUSTERDELEGATOR_H_INCLUDED' >> $OUTFILE
echo '#define CLUSTER_DELEGATOR_IS_AMALGAMATION' >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Message.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Communicator.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Coordinator_Worker_Process.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/Coordinator_Worker_Scheme.h >> $OUTFILE
echo >> $OUTFILE && cat include/delegator/delegator.h >> $OUTFILE
echo >> $OUTFILE && echo '#endif' >> $OUTFILE
