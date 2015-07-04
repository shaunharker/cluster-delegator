/// Coordinator_Worker_Process.hpp
/// Shaun Harker 
/// 2011
#ifndef CLUSTER_DELEGATOR_COORDINATOR_WORKER_PROCESS_HPP
#define CLUSTER_DELEGATOR_COORDINATOR_WORKER_PROCESS_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "Coordinator_Worker_Process.h"
#endif

INLINE_IF_HEADER_ONLY Coordinator_Worker_Process::
~Coordinator_Worker_Process( void )  { }

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Process::
command_line ( int argcin, char * argvin [] ) {
  argc = argcin;
  argv = argvin;
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Process::
initialize ( void ) {}

INLINE_IF_HEADER_ONLY int Coordinator_Worker_Process::
prepare ( Message & job_message ) {
  if ( JOBS_TO_SEND . empty () ) return 1;
  job_message = JOBS_TO_SEND . top ();
  JOBS_TO_SEND . pop ();
  return 0;
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Process::
work ( Message & result_message, 
       Message const& job_message ) const {}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Process::
accept ( Message const& results ) {
  RESULTS_RECEIVED . push ( results );
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Process::
finalize ( void ) {}

#endif
