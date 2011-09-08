/*
 *  main.cpp   Shaun Harker
 *  Example 1 of delegator project
 *  Illustrates "initialize-work-finalize" method
 */

// Feel free to strip my name off this particular file if you modify it to
// make your own program. -Shaun Harker

#include <iostream>
#include "delegator/delegator.h" 

void comment ( void ) {
  std::cout << "  This example generates pairs (i, j) where i and j differ by even amounts\n " 
  << "  and 0 <= i < j < 10. From these pairs it computes (s, t), where \n "
  << "            s = i + j \n " 
  << "            t = i * j \n "
  << "  The point of the example is to show the initialize/finalize method for \n"
  << "  arranging the computations.\n";
}

class Process : public Coordinator_Worker_Process {
public:
  void initialize ( void );
  void work ( Message & result_message, const Message & job_message ) const;
  void finalize ( void );
};

inline void Process::initialize ( void ) {
  comment ();
  std::cout << "  -------- Process::initialize ---------- \n";
  // Prepare job messages and push them onto "JOBS_TO_SEND" stack.
  int N = 10;
  for ( int i = 0; i < N; ++ i ) {
    for ( int j = 0; j < N; ++ j ) {
      if ( i >= j ) continue;
      if ( ( (j - i) % 2 ) == 1 ) continue;
      std::pair < int, int > job ( i, j );
      Message job_message;
      job_message << job;
      JOBS_TO_SEND . push ( job_message );
      std::cout << " Job (" << job . first << ", " << job . second << ") \n";
    }
  }
}

inline void Process::work ( Message & result_message, const Message & job_message ) const {
  // Extract job information from "job" message
  std::pair < int, int > job;
  job_message >> job;
  // Compute something interesting
  std::pair < int, int > result;
  result . first = job . first + job . second; 
  result . second = job . first * job . second;
  // Push results into "results" message
  result_message << job;
  result_message << result;
}

inline void Process::finalize ( void ) {
  std::cout << "  -------- Process::finalize ---------- \n";
  // Read result messages off from "RESULTS_RECEIVED" stack.
  while ( not RESULTS_RECEIVED . empty () ) {
    Message & result_message = RESULTS_RECEIVED . top ();
    std::pair < int, int > job, result;
    result_message >> job;
    result_message >> result;
    RESULTS_RECEIVED . pop ();
    std::cout << " Job (" << job . first << ", " << job . second << ") : " 
    << " Result (" << result . first << ", " << result . second << ")\n";    
  }  
}

/* main */
int main ( int argc, char * argv [] ) {
	delegator::Start ();
	delegator::Run < Process > ();
	delegator::Stop ();
	return 0;
}
