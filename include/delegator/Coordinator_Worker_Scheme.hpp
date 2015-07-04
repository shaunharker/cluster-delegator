/// Coordinator_Worker_Scheme.hpp
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_COORDINATOR_WORKER_SCHEME_HPP
#define CLUSTER_DELEGATOR_COORDINATOR_WORKER_SCHEME_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator/Coordinator_Worker_Scheme.h"
#include "delegator/Message.h"
#endif

#include <thread>
#include <chrono>
#include <mutex>

#ifndef CD_SLEEP_TIME
#define CD_SLEEP_TIME std::chrono::microseconds(10)
#endif

#ifndef CD_LOG
#define CD_LOG(X) mtx.lock(); std::cout << X ; mtx.unlock();
#endif

#ifndef CD_TICKTIME
#define CD_TICKTIME ((double)(std::chrono::system_clock::now().time_since_epoch().count()%1000000000LL))/1000000.0
#endif

INLINE_IF_HEADER_ONLY Coordinator_Worker_Scheme::
Coordinator_Worker_Scheme ( int argc, char * argv [] ) 
: argc ( argc ), argv ( argv ), received(0), sent(0) {}

/* Coordinator_Worker_Scheme::run() */
INLINE_IF_HEADER_ONLY void Coordinator_Worker_Scheme::
run ( Coordinator_Worker_Process * process,
      Communicator * communicator ) {
  my_process = process;
  my_communicator = communicator;
  // Set the command line arguments for the process
  my_process -> command_line ( argc, argv );
  // Determine if Coordinator or Worker
  if ( my_communicator -> coordinating () ) {
    run_coordinator ();
  } else {
    run_worker ();
  }
  my_communicator -> halt ();
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Scheme::
coordinator_incoming ( void ) {
  while ( not done ) { 
    // Check if expecting more results
    mtx . lock ();
    bool expecting_more_results = not out_of_jobs || 
                                  prepared . size () > 0 || 
                                  received < sent;
    mtx . unlock ();
    // If not expecting more results, then we are done.
    if ( not expecting_more_results ) { 
      mtx . lock ();
      done = true;
      mtx . unlock ();
      break;
    }
    // Receive one of the expected results
    Message incoming;
    Channel worker;
    my_communicator -> receive ( &incoming, 
                                 &worker );
    // Not actually a result, just a ready worker.
    if ( incoming . tag == READY ) { 
      mtx . lock ();
      ready . push ( worker );
      mtx . unlock ();
    }
    // Accept the result
    if ( incoming . tag == RESULTS ) { 
      // Record receipt
      mtx . lock ();
      my_process -> accept ( incoming );
      ++ received;
      mtx . unlock ();
    }

  }
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Scheme::
coordinator_outgoing ( void ) {
  while ( not done ) {
    // Check if there is a worker available and a job ready.
    mtx . lock ();
    bool job_and_worker_ready = ( ready . size () > 0 && prepared . size () > 0 );
    mtx . unlock ();
    // If there isn't, sleep for 1ms to avoid contention on the lock
    if ( not job_and_worker_ready ) {
      std::this_thread::sleep_for(CD_SLEEP_TIME);
      continue;
    }
    // Obtain the identity of the available worker and the job message
    mtx . lock ();
    Message job = prepared . front ();
    Channel worker = ready . front ();
    prepared . pop ();
    ready . pop ();
    ++ sent;
    mtx . unlock ();
    // Send the job to the worker
    my_communicator -> send ( job, worker );
  }
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Scheme::
coordinator_preparing ( void ) {
  while ( not out_of_jobs ) {
    // See if there is a need to prepare more jobs (i.e. too few ready)
    mtx . lock ();
    bool need_to_prepare_more_jobs = ( prepared . size () < 2 );
    mtx . unlock ();
    // If no need to prepare jobs, sleep for 1ms to avoid contention on the lock
    if ( not need_to_prepare_more_jobs ) { 
      std::this_thread::sleep_for(CD_SLEEP_TIME);
      continue;
    }
    // Prepare the job
    Message job;
    mtx . lock ();
    int prepare_status = my_process -> prepare ( job );
    job . tag = 1;
    mtx . unlock ();
    switch ( prepare_status ) {
      case 0: // Job prepared
        mtx . lock ();
        prepared . push ( job );
        mtx . unlock ();
        break;
      case 1: // No jobs remain
        mtx . lock ();
        out_of_jobs = true;
        mtx . unlock ();
        break;
      case 2: // Jobs remain, but not yet available
        std::this_thread::sleep_for(CD_SLEEP_TIME);
        break;
    }
  }
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Scheme::
run_coordinator ( void ) {
  done = false; out_of_jobs = false;
  // Call the user's "initialize" method
  my_process -> initialize ();
  // Start a thread to deal with incoming messages
  std::thread t1(&Coordinator_Worker_Scheme::coordinator_incoming, this );
  // Start a thread to send job messages
  std::thread t2(&Coordinator_Worker_Scheme::coordinator_outgoing, this );
  // Start a thread to prepare job messages
  coordinator_preparing ();
  t1.join();
  t2.join();
  // Call the user's "finalize" method
  my_process -> finalize ();
}

INLINE_IF_HEADER_ONLY void Coordinator_Worker_Scheme::
run_worker ( void) {
  Channel boss = my_communicator -> DIRECTOR;
  while ( true ) {
    // Send a READY message.
    Message ready;
    ready . tag = READY;
    my_communicator -> send ( ready, boss );
    // Receive a job.
    Message job_message;
		my_communicator -> receive ( &job_message, &boss );
    /* Quit if it is a retire job. */
    if ( job_message . tag == RETIRE ) {
      break;
    }
    // Work the job.
    Message result_message;
    my_process -> work ( result_message, job_message );
		result_message . tag = RESULTS;
    // Send the results back to the coordinator.
    my_communicator -> send ( result_message, boss );
  }
}

#endif
