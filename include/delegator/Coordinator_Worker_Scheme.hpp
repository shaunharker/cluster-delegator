/// Coordinator_Worker_Scheme.hpp
/// Shaun Harker 
/// 2011

#include <thread>
#include <chrono>
#include <mutex>

/************************************************
 *        Coordinator_Worker_Scheme             *
 ************************************************/

inline Coordinator_Worker_Scheme::
Coordinator_Worker_Scheme ( int argc, char * argv [] ) 
: argc ( argc ), argv ( argv ), received(0), sent(0) {
}

/* Coordinator_Worker_Scheme::run() */
inline void Coordinator_Worker_Scheme::
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

inline void Coordinator_Worker_Scheme::
coordinator_incoming ( void ) {
  //std::cout << "coordinator incoming\n";
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
      std::cout << "coordinator_incoming exit criteria:\n";
      std::cout << (out_of_jobs ? "out_of_jobs" : "not_out_of_jobs" ) << "\n";
      std::cout << prepared.size () << " == 0\n";
      std::cout << received << " == " << sent << "\n";
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
      my_process -> accept ( incoming );
      // Record receipt
      mtx . lock ();
      ++ received;
      mtx . unlock ();
    }

  }
}

inline void Coordinator_Worker_Scheme::
coordinator_outgoing ( void ) {
  //std::cout << "coordinator outgoing\n";
  while ( not done ) {
    // Check if there is a worker available and a job ready.
    mtx . lock ();
    bool job_and_worker_ready = ( ready . size () > 0 && prepared . size () > 0 );
    //std::cout << ready.size () << " " << prepared.size () << "?\n";
    mtx . unlock ();
    // If there isn't, sleep for 1ms to avoid contention on the lock
    if ( not job_and_worker_ready ) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    //std::cout << "coordinator outgoing send\n";
    // Obtain the identity of the available worker and the job message
    mtx . lock ();
    Message job = prepared . top ();
    Channel worker = ready . top ();
    prepared . pop ();
    ready . pop ();
    ++ sent;
    mtx . unlock ();
    // Send the job to the worker
    my_communicator -> send ( job, worker );
  }
}

inline void Coordinator_Worker_Scheme::
coordinator_preparing ( void ) {
  //std::cout << "coordinator preparing\n";
  while ( not out_of_jobs ) {
    // See if there is a need to prepare more jobs (i.e. too few ready)
    mtx . lock ();
    bool need_to_prepare_more_jobs = ( prepared . size () < 2 );
    mtx . unlock ();
    // If no need to prepare jobs, sleep for 1ms to avoid contention on the lock
    if ( not need_to_prepare_more_jobs ) { 
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    // Prepare the job
    Message job;
    int prepare_status = my_process -> prepare ( job );
    switch ( prepare_status ) {
      case 0: // Job prepared
        //std::cout << "coordinator_preparing job prepared\n";
        mtx . lock ();
        prepared . push ( job );
        mtx . unlock ();
        break;
      case 1: // No jobs remain
        //std::cout << "coordinator_preparing no jobs remain\n";
        mtx . lock ();
        out_of_jobs = true;
        mtx . unlock ();
        break;
      case 2: // Jobs remain, but not yet available
        //std::cout << "coordinator_preparing sleep\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        break;
    }
  }
}

inline void Coordinator_Worker_Scheme::
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


/* Coordinator_Worker_Process::run_worker() */
inline void Coordinator_Worker_Scheme::
run_worker ( void) {
  Channel boss = my_communicator -> DIRECTOR;
  while ( true ) {
    /* Send a READY message. */
    Message ready;
    ready . tag = READY;
    my_communicator -> send ( ready, boss );

    /* Receive a job. */
    Message job_message;
		my_communicator -> receive ( &job_message, &boss );
    /* Quit if it is a retire job. */
    if ( job_message . tag == RETIRE ) {
      break;
    }

    /* Work the job. */
    Message result_message;
    my_process -> work ( result_message, job_message );
		result_message . tag = RESULTS;

    /* Send the results back to the coordinator. */
    my_communicator -> send ( result_message, boss );
  }
}
