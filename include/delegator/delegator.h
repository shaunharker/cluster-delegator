/// delegator.h
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_H
#define CLUSTER_DELEGATOR_H

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator/Message.hpp"
#include "delegator/Communicator.hpp"
#include "delegator/Coordinator_Worker_Scheme.hpp"
#include "delegator/Coordinator_Worker_Process.hpp"
#endif

namespace delegator {
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

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator/delegator.hpp"
#endif

#endif
