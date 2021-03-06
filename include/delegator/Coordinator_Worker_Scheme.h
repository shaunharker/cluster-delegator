/// Coordinator_Worker_Scheme.h
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_COORDINATOR_WORKER_SCHEME_H
#define CLUSTER_DELEGATOR_COORDINATOR_WORKER_SCHEME_H

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator/Message.h"
#include "delegator/Communicator.h"
#include "delegator/Coordinator_Worker_Process.h"
#endif
#include <mutex>
#include <cstdint>
#include <queue>

/// Coordinator_Worker_Scheme
class Coordinator_Worker_Scheme {
public:
  Coordinator_Worker_Scheme ( int argc, char * argv [] );
  void run ( Coordinator_Worker_Process * my_process, 
             Communicator * my_communicator );
private:    
  int argc; char * * argv;
  static const int RETIRE = 0;
  static const int JOB = 1;
  static const int READY = 2;
  static const int RESULTS = 3;
  Coordinator_Worker_Process * my_process;
  Communicator * my_communicator;
  void run_coordinator ( void );
  void run_worker ( void );
  void coordinator_incoming ( void );
  void coordinator_outgoing ( void );
  void coordinator_preparing ( void );
  bool out_of_jobs;
  bool done;
  uint64_t received;
  uint64_t sent;
  std::queue<Message> prepared;
  std::queue<Channel> ready;
  std::mutex mtx;
};

#endif
