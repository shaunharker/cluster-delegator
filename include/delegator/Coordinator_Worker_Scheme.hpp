/*
 *  Coordinator_Worker_Scheme.hpp
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/************************************************
 *        Coordinator_Worker_Scheme             *
 ************************************************/

#ifndef CD_LOGGING_ON
#define CD_LOGGING_ON if(0)
#endif
 
inline Coordinator_Worker_Scheme::Coordinator_Worker_Scheme ( int argc, char * argv [] ) 
: argc ( argc ), argv ( argv ) {}

/* Coordinator_Worker_Scheme::run() */
inline void Coordinator_Worker_Scheme::run ( Coordinator_Worker_Process * my_process,
                                             Communicator * my_communicator ) {
  // Set the command line arguments for the process
  my_process -> command_line ( argc, argv );
  // Determine if Coordinator or Worker
  if ( my_communicator -> coordinating () ) {
    run_coordinator ( my_process, my_communicator );
  } else {
    run_worker ( my_process, my_communicator );
  }
}

/* Coordinator_Worker_Scheme::run_coordinator() */
inline void Coordinator_Worker_Scheme::run_coordinator ( Coordinator_Worker_Process * my_process,
                                                         Communicator * my_communicator ) {
  CD_LOGGING_ON std::cout << "COORD: Initialize.\n";
  my_process -> initialize ();
  int pending = 0;
  bool retiring = false;  
	while ( not retiring || pending > 0 ) { 
		/* Receive a message */
    CD_LOGGING_ON std::cout << "COORD: Receive a message.\n";
    Message incoming;
    Channel worker;
		my_communicator -> receive ( &incoming, 
                                 &worker, 
                                 my_communicator -> ANYTAG, 
                                 my_communicator -> ANYSOURCE );
		/* Deal with READY. */
		if ( incoming . tag == READY ) {
      CD_LOGGING_ON std::cout << "COORD: Deal with READY.\n";
      Message job_message;
      /* Prepare a job. (if we cannot, break )*/
      int prepare_status = my_process -> prepare ( job_message );
			/* Check the status of prepare and act accordingly. */
      if ( prepare_status == 0 )  {
				job_message . tag = JOB;
        ++ pending;
        my_communicator -> send ( job_message, worker );
			} else {
        retiring = true;
      }
		} /* if */
		
		/* Deal with a RESULTS message */
		if ( incoming . tag == RESULTS ) {
      CD_LOGGING_ON std::cout << "COORD: Deal with RESULTS.\n";
      -- pending;
      my_process -> accept ( incoming ); 
    }
  }
  
  // broadcast retire message
  CD_LOGGING_ON std::cout << "COORD: Broadcast RETIRE.\n";
  Message retire_message;
  my_process -> prepare ( retire_message );
  retire_message . tag = RETIRE;
  my_communicator -> broadcast ( retire_message );
  
  CD_LOGGING_ON std::cout << "COORD: Finalize.\n";
  my_process -> finalize ();
}

/* Coordinator_Worker_Process::run_worker() */
inline void Coordinator_Worker_Scheme::run_worker ( Coordinator_Worker_Process * my_process,
                                                    Communicator * my_communicator ) {
  /* Main loop */
  Channel boss = my_communicator -> DIRECTOR;
  while ( true ) {
    /* Send a READY message. */
    //std::cout << "Worker_Base::run -- send a READY message.\n";
#ifdef USEIDLEWORKERS
    if ( my_communicator -> SELF <= NUMBER_OF_WORKERS ) {
#endif
    Message ready;
    ready . tag = READY;
    my_communicator -> send ( ready, boss );
#ifdef USEIDLEWORKERS
    }
#endif
    /* Receive a job. */
    //std::cout << "Worker_Base::run -- receive a job\n";
    Message job_message;
		my_communicator -> receive ( &job_message, &boss, 
                                 my_communicator -> ANYTAG, my_communicator -> ANYSOURCE );
    /* Quit if it is a retire job. */
    if ( job_message . tag == RETIRE ) {
      //std::cout << "Worker_Base::run -- retiring\n";
      break;
    }
    /* Work the job. */
    //std::cout << "Worker_Base::run -- work a job\n";
    Message result_message;
    my_process -> work ( result_message, job_message );
		result_message . tag = RESULTS;
    /* Send the results back to the coordinator. */
    //std::cout << "Worker_Base::run -- report results\n";
    my_communicator -> send ( result_message, boss );
  }
}
