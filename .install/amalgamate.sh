#!/bin/bash

SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR/..
mkdir -p dist
OUTFILE=./dist/cluster-delegator.hpp

echo '/// cluster-delegator amalgated header' > $OUTFILE
echo '/*' >> $OUTFILE
cat LICENSE >> $OUTFILE
echo '*/' >> $OUTFILE
echo '#ifndef CLUSTERDELEGATOR_H_INCLUDED' >> $OUTFILE
echo '#define CLUSTERDELEGATOR_H_INCLUDED' >> $OUTFILE
echo '#define CLUSTER_DELEGATOR_IS_AMALGAMATION' >> $OUTFILE
cat include/delegator/Message.h >> $OUTFILE
cat include/delegator/Message.hpp >> $OUTFILE
cat include/delegator/Communicator.h >> $OUTFILE
cat include/delegator/Communicator.hpp >> $OUTFILE
cat include/delegator/Coordinator_Worker_Process.h >> $OUTFILE
cat include/delegator/Coordinator_Worker_Process.hpp >> $OUTFILE
cat include/delegator/Coordinator_Worker_Scheme.h >> $OUTFILE
cat include/delegator/Coordinator_Worker_Scheme.hpp >> $OUTFILE
cat include/delegator/delegator.h >> $OUTFILE
cat include/delegator/delegator.hpp >> $OUTFILE
echo '#endif' >> $OUTFILE
