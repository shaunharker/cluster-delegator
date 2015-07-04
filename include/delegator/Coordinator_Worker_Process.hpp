/// Coordinator_Worker_Process.hpp
/// Shaun Harker 
/// 2011

inline Coordinator_Worker_Process::
~Coordinator_Worker_Process( void )  { }

inline void Coordinator_Worker_Process::
command_line ( int argcin, char * argvin [] ) {
  argc = argcin;
  argv = argvin;
}

inline void Coordinator_Worker_Process::
initialize ( void ) {}

inline int Coordinator_Worker_Process::
prepare ( Message & job_message ) {
  if ( JOBS_TO_SEND . empty () ) return 1;
  job_message = JOBS_TO_SEND . top ();
  JOBS_TO_SEND . pop ();
  return 0;
}

inline void Coordinator_Worker_Process::
work ( Message & result_message, 
       Message const& job_message ) const {}

inline void Coordinator_Worker_Process::
accept ( const Message & results ) {
  RESULTS_RECEIVED . push ( results );
}

inline void Coordinator_Worker_Process::
finalize ( void ) {}
