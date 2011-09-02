/*
 *  delegator.hpp
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set>
#include <deque>
#include <unistd.h>

/***************************************
 *         USER INTERFACE              *
 ***************************************/

namespace delegator {
  
  void Start ( void ) {
  	/* Initialize the MPI communications */
  	int argc; char * * argv;
		MPI_Init(&argc, &argv); 
  }
  
  template < class Process >
  int Run ( void ) {
    int argc = 0; char * * argv = NULL;
    return Run < Process > ( argc, argv );
  } /* Run<> */

  void Stop ( void ) {
  	/* Finalize the MPI communications. */
		MPI_Finalize();
  }
  
  template < class Process >
  int Run ( int argc, char * argv [] ) {
    typedef Coordinator_Worker_Scheme Scheme;
    return Run < Process, Scheme, Communicator > ( argc, argv );
  } /* Start<> */

  // RunDelegator (more advanced interface)
  template < class Process, class Scheme, class Comm >
  int Run ( int argc, char * argv [] ) {
    /* Create Process, Scheme, and Communicator */
    Comm my_communicator;
    Scheme my_scheme ( argc, argv );
    Process my_process;
    
    // Run Scheme until it finishes
    my_communicator . initialize ();
    my_scheme . run ( & my_process, & my_communicator );
    my_communicator . finalize ();
    return 0; 
  } /* RunDelegator<> */
}


