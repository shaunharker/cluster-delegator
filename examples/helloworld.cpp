// Hello World! example
// Shaun Harker
// 2015-07-04

#include "cluster-delegator.hpp"
#include <string>
#include <vector>

class Process : public Coordinator_Worker_Process {
public:
  void command_line ( int argcin, char * argvin [] ) {
    std::cout << "This is called by every process at start!\n";
  }
  void initialize ( void ) {
    std::cout << "This is called by the coordinator before jobs start!\n";
    current_job_number = 0;
    names = { "Shaun", "Pawel", "Ippei", "World" };
    total_number_of_jobs = names . size ();
  }
  int prepare ( Message & job_message ) {
    std::cout << "This is called by the coordinator to produce jobs!\n";
    if ( current_job_number == total_number_of_jobs ) return 1; // Code 1: No more jobs!
    job_message << names [ current_job_number ++ ];   return 0; // Code 0: A job was sent!
  }
  void work ( Message & result_message, Message const& job_message ) const {
    std::cout << "This method is called by a worker receiving a job!\n";
    std::string name; job_message >> name;
    std::string greeting = "Hello " + name + "!";
    result_message << greeting;
  }
  void accept ( Message const& result_message ) {
    std::cout << "This is called by the coordinator to process a result!\n";
    std::string data; result_message >> data;
    std::cout << data << "\n";
  }
  void finalize ( void ) { 
    std::cout << "This is called by the coordinator when all jobs are done!\n";
  }
private:
  // Each process has its own copy of the member variables below:
  int current_job_number;
  int total_number_of_jobs;
  std::vector<std::string> names;
};

int main ( int argc, char * argv [] ) {
  delegator::Start ();
  delegator::Run < Process > (argc, argv); 
  delegator::Stop ();
  return 0;
}
