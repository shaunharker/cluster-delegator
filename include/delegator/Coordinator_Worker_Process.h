/*
 *  Coordinator_Worker_Scheme.h
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef SRH_COORDINATOR_WORKER_PROCESS_H
#define SRH_COORDINATOR_WORKER_PROCESS_H

#include "delegator/Message.h"
#include <stack>

/************************************************
 *          Coordinator_Worker_Process          *
 ************************************************/

/** Coordinator_Worker_Process */
class Coordinator_Worker_Process {
protected:
  int argc; char * * argv;
  std::stack < Message > JOBS_TO_SEND;
  std::stack < Message > RESULTS_RECEIVED;
public:
  /** Constructors */
  virtual ~Coordinator_Worker_Process ( void );

  /** void command_line ( int argc, char * argv []). 
      Set command line arguments. */
  virtual void command_line ( int argcin, char * argvin [] );
  
  /** Initialize. */
  virtual void initialize ( void );

  /** Prepare a job to send. 
   * Return value is 0 if job is prepared.
   * Return value is 1 if a retirement job is prepared (no work left.)
   * Return value is 2 if no job prepared because none available until 
   *              more results are processed.
   */
  virtual int prepare ( Message & job_message );
  
  /** Work the job. */
  virtual void work ( Message & result_message, const Message & job_message ) const;

  /** Receive the result of a job. */
  virtual void accept ( const Message & result_message );
  
  /** Finalize. */
  virtual void finalize ( void );
  
};

#include "delegator/Coordinator_Worker_Process.hpp"

#endif
