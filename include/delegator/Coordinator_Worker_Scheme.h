/*
 *  Coordinator_Worker_Scheme.h
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef SRH_COORDINATOR_WORKER_SCHEME_H
#define SRH_COORDINATOR_WORKER_SCHEME_H

#include "delegator/Message.h"
#include "delegator/Communicator.h"
#include "delegator/Coordinator_Worker_Process.h"

/************************************************
 *        Coordinator_Worker_Scheme             *
 ************************************************/

/** Coordinator_Worker_Scheme */
class Coordinator_Worker_Scheme {
public:
  Coordinator_Worker_Scheme ( int argc, char * argv [] );
  void run ( Coordinator_Worker_Process * my_process, 
             Communicator * my_communicator );
private:    
  int argc; char * * argv;
  static const int JOB = 0;
  static const int RETIRE = 1;
  static const int READY = 2;
  static const int RESULTS = 3;
  void run_coordinator ( Coordinator_Worker_Process * my_process,
                         Communicator * my_communicator );
  void run_worker ( Coordinator_Worker_Process * my_process,
                    Communicator * my_communicator );
};

#include "delegator/Coordinator_Worker_Scheme.hpp"

#endif
