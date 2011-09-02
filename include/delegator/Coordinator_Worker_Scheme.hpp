/*
 *  Coordinator_Worker_Scheme.hpp
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set>
#include <deque>
#include <unistd.h>

/************************************************
 *        Coordinator_Worker_Scheme             *
 ************************************************/

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
  
  /*
   * Essential idea: try to minimize time workers are not doing jobs.
   * Strategy: keep a queue of workers, a queue of jobs, and a queue of results to process
   * Fill the job queue before moving onto processing jobs.
   * If a message is received at any point, it is either RESULTS or READY.
   * If RESULTS, add to queue. If READY, give a job immediately if possible, or else add to worker queue.
   *
   * Notes: At any given time, either the job queue or the worker queue is empty.
   *        The strategy of filling the entire job queue before processing results may be questionable.
   *        The rationale is that we don't want to be caught without a job by a free worker.
   *
   *        It may be that only a certain number of results should be held unprocessed.
   *        Eventually, they are clearly building up and should be dealt with -- this is a bad
   *        scenario we call "swamping". We control this with the RESULTS_LIMIT define
   *
   *        Similarly, we might decide that a certain number of extra jobs kept ahead of time
   *        is gratuitous. We control this with the JOB_LIMITS
   *
   *
   */
  my_process -> initialize ();
  
  usleep ( 100000 ); // give some time for workers to wake up
  
  unsigned int JOB_LIMIT = 256;
  unsigned int RESULTS_LIMIT = 256;
  
  int number_of_workers = 0;
  bool no_more_jobs = false;
  
	/* Initialize data structures. */
	std::deque<Channel> worker_queue;
	std::deque<Message> job_queue;
	std::deque<Message> results_queue;
	std::set<Channel> unretired_workers;
  
	do {
    
		//std::cout << "Coordinator: Job queuing loop.\n";
    /* JOB QUEUING LOOP  
     Make jobs and give them to available workers until either
     a) The limit of jobs we are willing to store is met
     b) A message is available 
     c) We must await results before computing the next job */
    
    // While there is no message and we can spend our time making new jobs...
		while ( not my_communicator -> probe ( my_communicator -> ANYTAG ) && job_queue . size () < JOB_LIMIT ) {
      Message job_message;
      /* Prepare a job. (if we cannot, break )*/
      //std::cout << "Coordinator_Base::run --- preparing a job\n";
      int prepare_status = my_process -> write ( job_message );
			
			/* Check the status of prepare and act accordingly. */
			
			/* Has prepare created a job for us? */
			if ( prepare_status == 0 )  {
				job_message . tag = JOB;
			} /* if */
			
			/* Does prepare say it's time to retire? */
			if ( prepare_status == 1 )  {
				job_message . tag = RETIRE; 
				/* If there are no workers in line to retire, exit the job-queueing loop.*/
				if ( worker_queue . empty () ) {
					no_more_jobs = true; 
					break;
				} /* if */
			} /* if */
			
			/* Does prepare say jobs cannot be created for now? If so, exit the job-queueing loop. */
			if ( prepare_status == 2 ) break;
      
			/* Check if there is a worker to give the job to. */
			if ( not worker_queue . empty () ) {
        /* Get a worker from the front of the queue. */
				Channel & worker =  worker_queue . front ();
        /* Give the job to the worker. */
        //std::cout << "Coordinator_Base::run --- sending a job\n";
        my_communicator -> send ( job_message, worker );
        /* The worker is no longer in line. */
        worker_queue . pop_front ();
        /* If worker is being retired, take note. */
        if ( job_message . tag == RETIRE ) {
          unretired_workers . erase ( worker );
          --number_of_workers;
          no_more_jobs = true;
        } /* if */
			} else {
        /* There aren't any workers to take the job. Put it the job queue. */
				job_queue . push_back ( job_message ); 
			} /* if-else */
		} /* while */
    
    // AT THIS POINT:
    //   * either a message has arrived 
    //           OR
    //   * the jobs queue is full
    //
    
		//std::cout << "Coordinator: Processing loop.\n";
		/* Process results until finished or until a message comes */
		while ( not my_communicator -> probe ( my_communicator -> ANYTAG ) && results_queue . size () > 0 ) {
      /* Get a message from the queue. */
      Message & result_message = results_queue . front ();
      /* Process the message */
      //std::cout << "Coordinator_Base::run --- processing a result\n";
      my_process -> read ( result_message );
      /* The results message is no longer in the queue. */
      results_queue . pop_front ();
		} /* while */
		
    // AT THIS POINT:
    //   * either a message has arrived
    //          OR
    //   * there are no results to process
    //   * the job queue is full (perhaps with all retire jobs)
    
    //std::cout << "coordinator loop: check to see if we can leave\n";
		/* Determine whether or not to break from loop. Break if the work is done and nothing is left to process. */
		if ( number_of_workers == 0 && no_more_jobs && results_queue . empty () &&  
        not my_communicator -> probe ( my_communicator -> ANYTAG ) ) break;
		
    // AT THIS POINT:
    //   * there are no results to process
    //   * the job queue is full (perhaps with all retire jobs)
    //   * there will eventually be incoming messages, if there isn't one ready already
    
    // If there isn't a message available right away, sleep for a little bit
    //std::cout << "coordinator loop: check to see if we can sleep\n";

    if ( not my_communicator -> probe ( my_communicator -> ANYTAG ) ) {
      usleep ( 100000 );
    }
    
    //std::cout << "Coordinator_Base::run --- receiving a result\n";

    // AT THIS POINT:
    //   * We want to deal with receiving messages
    //   * We will collect these results until there are no more pending messages
    //   * We will hand out jobs to workers who give READY messages as they are available
    //   * If we run out of jobs, but still have READY workers, we start putting them on a queue
    
		/* Receive a message */
    Message incoming;
    Channel worker;
		my_communicator -> receive ( &incoming, &worker, my_communicator -> ANYTAG, my_communicator -> ANYSOURCE );
		
		//std::cout << "Coordinator: Deal with READY message if applicable.\n";
    
		/* Deal with a READY message */
		if ( incoming . tag == READY ) {
			++ number_of_workers;
      unretired_workers . insert ( worker );
			if ( not job_queue . empty () ) {
        /* Get a job from the front of the queue. */
				Message & job_message =  job_queue . front ();
        /* Give the job to the worker. */
        //std::cout << "Coordinator_Base::run --- sending a job (new ready worker)\n";
        my_communicator -> send ( job_message, worker );
        /* If worker is being retired, take note. */
        if ( job_message . tag == RETIRE ) {
          unretired_workers . erase ( worker );
          --number_of_workers;
          no_more_jobs = true;
        } /* if */
        /* The job is no longer in line. */
        job_queue . pop_front ();
			} else {
        /* Put the worker in line */
        worker_queue . push_back ( worker );
			} /* if-else */
		} /* if */
		
		//std::cout << "Coordinator: Deal with RESULTS message if applicable.\n";
		/* Deal with a RESULTS message */
		if ( incoming . tag == RESULTS ) {
			-- number_of_workers;
			if ( results_queue . size () < RESULTS_LIMIT ) {
        /* Put the results in queue to be processed later */
				//std::cout << "  Putting results in queue.\n";
        results_queue . push_back ( incoming );
      } else { /* Oh no! We are swamped. */
        /* In swamping condition we must process immediately */
				//std::cout << "  Swamped! Calling process.\n";
        //std::cout << "Coordinator_Base::run --- processing a result (swamp)\n";
        my_process -> read ( incoming );    
      } /* if-else */
    } /* if */
    
		//std::cout << "Coordinator: Bottom of loop.\n";
    
  } while ( number_of_workers > 0 || not no_more_jobs || not results_queue . empty () ||  
           my_communicator -> probe ( my_communicator -> ANYTAG ) );
  
	//std::cout << "Coordinator: Main Loop exited \n";
	
  /* All results have been processed. Tell all unretired workers to retire. */
  /* Create a retire message */
  Message retire_message;
  retire_message . tag = RETIRE;
  /* Send retire message out to all unretired workers. */
  for ( std::set<Channel>::const_iterator worker = unretired_workers . begin ();
       worker != unretired_workers . end (); ++ worker ) 
		my_communicator -> send ( retire_message, *worker );
  
	//std::cout << "Coordinator: Finished, returning. \n";
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
    Message ready;
    ready . tag = READY;
    my_communicator -> send ( ready, boss );
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
