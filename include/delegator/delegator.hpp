/// delegator.hpp
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_HPP
#define CLUSTER_DELEGATOR_HPP

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator.h"
#endif

#include <exception>
#include <stdexcept>
#include <thread>
#include <mpi.h>

namespace delegator {
  
  inline void Start ( void ) {
  	// Initialize the MPI communications
  	int argc = 0; char * * argv = NULL;
    int provided;
    int rc = MPI_Init_thread ( &argc, &argv, MPI_THREAD_MULTIPLE, &provided );
    if ( rc ) throw std::runtime_error ( "delegator::Start : MPI failed to initialize" );
  }
  
  template < class Process >
  int Run ( void ) {
    int argc = 0; char * * argv = NULL;
    return Run < Process > ( argc, argv );
  }

  inline void Stop ( void ) {
  	// Finalize the MPI communications.
		MPI_Finalize();
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
