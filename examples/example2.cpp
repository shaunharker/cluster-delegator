/*
 *  main.cpp    Shaun Harker
 *  Example 2 of distrib project 
 *  This example is intended to show full use of all interfaces 
 *       initialize, write, work, read, and finalize as well as 
 *       command line argument usage
 *  build and run instructions:
 *  make clean
 *  make
 *  mpiexec -np 8 ./example2 42 is the answer
 */

// Feel free to strip my name off this particular file if you modify it to
// make your own program. -Shaun Harker

#include <iostream>
#include <string>
#include "delegator/delegator.h"

void comment ( void ) {
  std::cout << "  This example generates a sequence of integers 0 <= i < n\n "
            << "  as jobs. For results, it returns the ith command line argument. \n"
            << "  If there are less than 10 arguments, it uses modular arithmetic.\n"
            << "  n is chosen to be the first given command line argument.\n"
            << "  The point of the example is to show how to override \n "
            << "  read and write methods of Coordinator_Worker_Process,\n" 
            << "  and also point out that command line arguments are available\n";
}

class Process : public Coordinator_Worker_Process {
public:
  void initialize ( void );
  int prepare ( Message & job_message );  
  void work ( Message & result_message, const Message & job_message ) const;
  void accept ( const Message & result_message );
  void finalize ( void );
private:
  int num_jobs_sent;
  int num_jobs;
};

inline void Process::initialize ( void ) {
  comment ();
  std::cout << "  -------- Process::initialize ---------- \n";
  num_jobs_sent = 0;
  if ( argc < 2 ) {
    std::cout << "First command line argument should be number of jobs.\n";
    num_jobs = 0;
    return;
  }
  num_jobs = atoi ( argv [ 1 ] ); // convert argument to integer "a - to - i"
  if ( num_jobs < 0 || num_jobs > 100 ) {
    std::cout << "Choose between 0 and 100 jobs.\n";
    num_jobs = 0;
    return;
  }
  std::cout << "  initialized with " << num_jobs << " jobs! \n";

}

inline int Process::prepare ( Message & job_message ) {
  // Check if done
  if ( num_jobs_sent == num_jobs ) return 1; //Finish
  // If not done, prepare next job
  int job_number = num_jobs_sent;
  job_message << job_number;
  ++ num_jobs_sent;
  std::cout << "Process::write --> Prepared job number " << job_number << "\n";
  return 0; // Prepared a job
}

void Process::work ( Message & result_message, 
                     const Message & job_message ) const {
  // Extract job information from "job" message
  int job;
  job_message >> job;
  // Compute something
  std::string result ( argv [ job % argc ] );
  // Push results into "results" message
  result_message << job;
  result_message << result;
  std::cout << "Process::work --> worked job " << job << "\n";
}

inline void Process::accept ( const Message & result_message ) {
  int job_number;
  std::string result;
  result_message >> job_number;
  result_message >> result;
  std::cout << "Process::read --> Job " << job_number 
            << " processed with result " << result << "\n";
}

inline void Process::finalize ( void ) {
  std::cout << "  -------- Process::finalize ---------- \n";
  std::cout << "  finished!\n";
}

// main 
int main ( int argc, char * argv [] ) {
	delegator::Start ();
	delegator::Run < Process > (argc, argv);
	delegator::Stop ();
	return 0;
}
