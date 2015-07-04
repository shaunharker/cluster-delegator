/// delegator.hpp
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_HPP
#define CLUSTER_DELEGATOR_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator.h"
#endif

#include <exception>
#include <stdexcept>
#include <thread>
#include <mpi.h>

namespace delegator {
  
  INLINE_IF_HEADER_ONLY void Start ( void ) {
  	// Initialize the MPI communications
  	int argc = 0; char * * argv = NULL;
    int provided;
    int rc = MPI_Init_thread ( &argc, &argv, MPI_THREAD_MULTIPLE, &provided );
    if ( rc ) throw std::runtime_error ( "delegator::Start : MPI failed to initialize" );
  }
  
  INLINE_IF_HEADER_ONLY void Stop ( void ) {
  	// Finalize the MPI communications.
		MPI_Finalize();
  }
  
}

#endif
