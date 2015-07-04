/// Coordinator_Worker_Process.h
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_COORDINATOR_WORKER_PROCESS_H
#define CLUSTER_DELEGATOR_COORDINATOR_WORKER_PROCESS_H

#include "delegator/Message.h"
#include <stack>

/// Coordinator_Worker_Process
///   Base class for user-defined process classes
class Coordinator_Worker_Process {
protected:
  int argc; char * * argv;
  std::stack < Message > JOBS_TO_SEND;
  std::stack < Message > RESULTS_RECEIVED;
public:
  /// virtual deconstructor
  virtual 
  ~Coordinator_Worker_Process ( void );

  /// command_line  
  ///   Set command line arguments
  ///   Called by all processes (coordinator and workers)
  virtual void 
  command_line ( int argcin, char * argvin [] );
  
  /// initialize
  ///   Called by coordinator before jobs begin
  virtual void 
  initialize ( void );

  /// prepare 
  ///   Prepare a job and store it in "job_message"
  ///   Return value is 0 if job is prepared.
  ///   Return value is 1 if a retirement job is prepared (no work left.)
  ///   Return value is 2 if no job prepared because none available until 
  ///              more results are processed.
  virtual int 
  prepare ( Message & job_message );
  
  /// work
  ///    Work the job in "job_message" and 
  ///    return result in "result_message"
  virtual void 
  work ( Message & result_message, 
         Message const& job_message ) const;

  /// accept
  ///   Process the returned result
  virtual void 
  accept ( const Message & result_message );
  
  /// finalize
  ///   Called by coordinator when all jobs finished
  virtual void 
  finalize ( void );
};

#include "delegator/Coordinator_Worker_Process.hpp"

#endif
