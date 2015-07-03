/// Communicator.hpp
/// Shaun Harker 
/// 2011

#include "mpi.h"
#include <string>
#include <stdexcept>
#include <exception>
#include <string>
#include <cstdint>
#include <algorithm>

#define MAX_MESSAGE_SIZE 1073741824L

#ifndef TICKTIME
#define TICKTIME ((double)(std::chrono::system_clock::now().time_since_epoch().count()%1000000000LL))/1000000.0
#endif

inline void Communicator::
initialize ( void ) {
	MPI_Comm_rank(MPI_COMM_WORLD, &SELF);
  DIRECTOR = 0;
}

inline void Communicator::
finalize ( void ) {
  // Shut-down message
  int numtasks;
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  int RETIRE = 0;
  for ( Channel channel = 1; channel < numtasks; ++ channel ) {
    MPI_Send ( NULL,0,MPI_CHAR,channel,RETIRE,MPI_COMM_WORLD );  
  }
}

inline void Communicator::
daemon ( void ) {

  //std::cout << "Communicator::daemon started\n";
  daemon_on . store ( true );
  while ( 1 ) {
    if ( not daemon_on . load () ) return;

    // Determine action
    int action = 0; // sleep (default)
    // Check if there is a message to receive
    MPI_Status status;
    int flag = 0;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    if ( flag ) action = 1; // receive

    if ( flag ) std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") receive flag\n";
    // Check if there is a message to send
    mtx . lock ();
    if ( not outbox . empty () ) action = 2; // send (overrides receive)
    mtx . unlock ();

    //if ( action > 0 ) std::cout << "ACTION = " << action << "\n";
    // Dispatch on chosen action
 
    switch ( action ) {
      case 0: // sleep
        mtx . lock ();
        std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") Sleep begin\n";
        mtx . unlock ();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        mtx . lock ();
        std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") Sleep end\n";
        mtx . unlock ();
        break;
      case 1: // receive
      {
        std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") receive begin\n";

        /* Get the length of the message */
        int buffer_length;
        int error_code = MPI_Get_count(&status, MPI_CHAR, &buffer_length);
        if ( error_code != 0 ) throw std::runtime_error("Communicator::receive Problem with message size");
        int tag = status . MPI_TAG;
        Channel channel = status . MPI_SOURCE;  
        mtx . lock ();
        std::string & text = incoming [ channel ];
        if ( buffer_length == 0 ) { 
          /* Edge case: message is integer multiple of GB size */
          std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") receiving packet (stub)\n";
          MPI_Recv ( NULL, 0, MPI_CHAR, channel, tag, MPI_COMM_WORLD, &status );
        } else {
          /* Allocate space for the message */
          uint64_t N = text . size ();
          text . resize ( N + (uint64_t) buffer_length );
          char * buffer = &text[N];
          /* Receive the message */
          std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") receiving packet\n";
          MPI_Recv ( buffer, buffer_length, MPI_CHAR, channel, tag, MPI_COMM_WORLD, &status );
        }
        if ( buffer_length < MAX_MESSAGE_SIZE ) { 
          std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") last packet; creating message\n";
          Message message;
          message . str ( text );
          message . tag = tag;
          inbox . push ( std::make_pair ( channel, message ) );
          text . clear ();
        }
        std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") receive end\n";
        mtx . unlock ();
        break;
      }
      case 2: // send message
      {
        // Retrieve the message
        mtx . lock ();
        std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") send begin\n";
        std::pair<Channel, Message> pair = outbox . front ();
        outbox . pop ();
        mtx . unlock ();
        Channel channel = pair . first;
        std::string text = pair . second . str ();
        int tag = pair . second . tag;
        uint64_t begin = 0;
        uint64_t N = text . size ();
        bool edge_case = false;
        if ( N == 0 ) edge_case = true;
        while ( N > 0 ) {
          int submessage_length = std::min ( (uint64_t) MAX_MESSAGE_SIZE, N );
          mtx . lock ();
          std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") sending packet of length " << submessage_length << "\n";
          mtx . unlock ();
          MPI_Send ( const_cast<char *> ( text . data () + begin ), 
                     submessage_length,
                     MPI_CHAR, 
                     channel, 
                     tag, 
                     MPI_COMM_WORLD );
          mtx . lock ();
          std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") finished sending packet of length " << submessage_length << "\n";
          mtx . unlock ();
          begin += submessage_length;
          N -= submessage_length;
          if ( submessage_length == MAX_MESSAGE_SIZE && N == 0 ) { 
            edge_case = true; // Message is integer GB in length
          }
        }
        /* Edge case must send addition zero-length message */
        if ( edge_case ) {
          mtx . lock ();
          std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") sending packet (stub)\n";
          mtx . unlock ();
          MPI_Send ( NULL,
                     0,
                     MPI_CHAR, 
                     channel, 
                     tag, 
                     MPI_COMM_WORLD ); 
        }
        mtx . lock ();
        std::cout << TICKTIME << " : Communicator::daemon (" << SELF << ") send end\n";
        mtx . unlock ();
        break;
      }
    }
  }
  //std::cout << "daemon exit!\n";
}

inline void Communicator::
send ( const Message & message, 
       const Channel & channel ) {
  mtx . lock ();
  outbox . push ( std::make_pair ( channel, message ) );
  std::cout << TICKTIME << " : Communicator::send (" << SELF << ") Pushing to outbox a message with tag " << message.tag << "\n";
  std::cout << TICKTIME << " : Communicator::send (" << SELF << ") Outbox is now non-empty (" << outbox.size() << " messages)\n";
  mtx . unlock ();
}

inline void Communicator::
receive ( Message * message, 
          Channel * channel ) {
  while ( 1 ) {
    mtx . lock ();
    if ( inbox . empty () ) { 
      mtx . unlock ();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    std::pair<Channel, Message> pair = inbox . front ();
    std::cout << TICKTIME << " : Communicator::receive (" << SELF << ") Inbox non-empty (" << inbox.size() << " messages)\n";
    std::cout << TICKTIME << " : Communicator::receive (" << SELF << ") Popping from inbox a message with tag " << pair.second.tag << "\n";
    inbox . pop ();
    mtx . unlock ();
    *channel = pair . first;
    *message = pair . second;
    return;
  }
} 

inline bool Communicator::
coordinating ( void ) {
  return ( SELF == DIRECTOR );
}

inline void Communicator:: 
halt ( void ) {
  daemon_on . store ( false );
}

