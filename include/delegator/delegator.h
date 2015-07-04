/// delegator.h
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_H
#define CLUSTER_DELEGATOR_H

#include "delegator/Message.h"
#include "delegator/Communicator.h"
#include "delegator/Coordinator_Worker_Scheme.h"

namespace delegator {
// Simplest interface: call start, then run, then stop.
  /// Start
  ///   Initialize the MPI world
  void 
  Start ( void );
  
  /// Run
  ///   Run the cluster-delegator process
  template < class Process > int 
  Run ( void );

  /// Stop
  ///   Finalize the MPI world
  void 
  Stop ( void );
  
  /// Run 
  ///   Run the cluster-delegator process
  ///   with the supplied command line arguments
  template < class Process > int 
  Run ( int argc, char * argv [] );
  
  /// Run                                                      
  ///     More advanced interface which can specify different  
  ///     Scheme or Communicator                               
  template < class Process, class Scheme, class Communicator > int 
  Run ( int argc, char * argv [] );
}

#include "delegator/delegator.hpp"

#endif
