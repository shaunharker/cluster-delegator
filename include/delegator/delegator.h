/// delegator.h
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_H
#define CLUSTER_DELEGATOR_H

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator/Message.h"
#include "delegator/Communicator.h"
#include "delegator/Coordinator_Worker_Scheme.h"
#include "delegator/Coordinator_Worker_Process.h"
#endif

#include <thread>

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

// Template definitions
namespace delegator {
  template < class Process >
  int Run ( void ) {
    int argc = 0; char * * argv = NULL;
    return Run < Process > ( argc, argv );
  }

  template < class Process >
  int Run ( int argc, char * argv [] ) {
    typedef Coordinator_Worker_Scheme Scheme;
    return Run < Process, Scheme, Communicator > ( argc, argv );
  }

  // Run (more advanced interface)
  template < class Process, class Scheme, class Comm >
  int Run ( int argc, char * argv [] ) {
    // Create Process, Scheme, and Communicator
    Comm my_communicator;
    Scheme my_scheme ( argc, argv );
    Process my_process;
    
    // Run Scheme until it finishes
    my_communicator . initialize ();
    std::thread t ( &Scheme::run, &my_scheme, &my_process, &my_communicator ); //my_scheme . run ( & my_process, & my_communicator );
    my_communicator . daemon ();
    t . join ();
    my_communicator . finalize ();
    return 0; 
  }
}
#endif
