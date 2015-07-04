/// Communicator.hpp
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_COMMUNICATOR_HPP
#define CLUSTER_DELEGATOR_COMMUNICATOR_HPP

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "Communicator.h"
#endif

#include <iostream>
#include <mpi.h>
#include <chrono>
#include <stdexcept>
#include <exception>
#include <cstdint>
#include <algorithm>
#include <thread>
#include <string>

#ifndef CD_MAX_MESSAGE_SIZE
#define CD_MAX_MESSAGE_SIZE 1073741824L
#endif

#define DEBUG if(0)

#ifndef CD_SLEEP_TIME
#define CD_SLEEP_TIME std::chrono::microseconds(10)
#endif

#ifndef CD_TICKTIME
#define CD_TICKTIME ((double)(std::chrono::system_clock::now().time_since_epoch().count()%1000000000LL))/1000000.0
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
#ifdef CD_COMM_LOGGING
    if ( flag ) std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") receive flag\n";
#endif
    // Check if there is a message to send
    mtx . lock ();
    if ( not outbox . empty () ) action = 2; // send (overrides receive)
    mtx . unlock ();

    // Dispatch on chosen action 
    switch ( action ) {
      case 0: // sleep
#ifdef CD_COMM_LOGGING
        mtx . lock ();
        std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") Sleep begin\n";
        mtx . unlock ();
#endif
        std::this_thread::sleep_for(CD_SLEEP_TIME);
#ifdef CD_COMM_LOGGING
        mtx . lock ();
        std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") Sleep end\n";
        mtx . unlock ();
#endif
        break;
      case 1: // receive
      {
#ifdef CD_COMM_LOGGING
        mtx . lock ();
        std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") receive begin\n";
        mtx . unlock ();
#endif
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
#ifdef CD_COMM_LOGGING
          std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") receiving packet (stub)\n";
#endif
          MPI_Recv ( NULL, 0, MPI_CHAR, channel, tag, MPI_COMM_WORLD, &status );
        } else {
          /* Allocate space for the message */
          uint64_t N = text . size ();
          text . resize ( N + (uint64_t) buffer_length );
          char * buffer = &text[N];
          /* Receive the message */
#ifdef CD_COMM_LOGGING
          std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") receiving packet\n";
#endif
          MPI_Recv ( buffer, buffer_length, MPI_CHAR, channel, tag, MPI_COMM_WORLD, &status );
        }
        if ( buffer_length < CD_MAX_MESSAGE_SIZE ) { 
#ifdef CD_COMM_LOGGING
          std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") last packet; creating message\n";
#endif
          Message message;
          message . str ( text );
          message . tag = tag;
          inbox . push ( std::make_pair ( channel, message ) );
          text . clear ();
        }
#ifdef CD_COMM_LOGGING
        std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") receive end\n";
#endif
        mtx . unlock ();
        break;
      }
      case 2: // send message
      {
        // Retrieve the message
        mtx . lock ();
#ifdef CD_COMM_LOGGING
        std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") send begin\n";
#endif
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
          int submessage_length = std::min ( (uint64_t) CD_MAX_MESSAGE_SIZE, N );
#ifdef CD_COMM_LOGGING
          mtx . lock ();
          std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") sending packet of length " << submessage_length << "\n";
          mtx . unlock ();
#endif
          MPI_Send ( const_cast<char *> ( text . data () + begin ), 
                     submessage_length,
                     MPI_CHAR, 
                     channel, 
                     tag, 
                     MPI_COMM_WORLD );
#ifdef CD_COMM_LOGGING
          mtx . lock ();
          std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") finished sending packet of length " << submessage_length << "\n";
          mtx . unlock ();
#endif
          begin += submessage_length;
          N -= submessage_length;
          if ( submessage_length == CD_MAX_MESSAGE_SIZE && N == 0 ) { 
            edge_case = true; // Message is integer GB in length
          }
        }
        /* Edge case must send addition zero-length message */
        if ( edge_case ) {
#ifdef CD_COMM_LOGGING
          mtx . lock ();
          std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") sending packet (stub)\n";
          mtx . unlock ();
#endif
          MPI_Send ( NULL,
                     0,
                     MPI_CHAR, 
                     channel, 
                     tag, 
                     MPI_COMM_WORLD ); 
        }
#ifdef CD_COMM_LOGGING
        mtx . lock ();
        std::cout << CD_TICKTIME << " : Communicator::daemon (" << SELF << ") send end\n";
        mtx . unlock ();
#endif
        break;
      }
    }
  }
}

inline void Communicator::
send ( const Message & message, 
       const Channel & channel ) {
  mtx . lock ();
  outbox . push ( std::make_pair ( channel, message ) );
#ifdef CD_COMM_LOGGING
  std::cout << CD_TICKTIME << " : Communicator::send (" << SELF << ") Pushing to outbox a message with tag " << message.tag << "\n";
  std::cout << CD_TICKTIME << " : Communicator::send (" << SELF << ") Outbox is now non-empty (" << outbox.size() << " messages)\n";
#endif
  mtx . unlock ();
}

inline void Communicator::
receive ( Message * message, 
          Channel * channel ) {
  while ( 1 ) {
    mtx . lock ();
    if ( inbox . empty () ) { 
      mtx . unlock ();
      std::this_thread::sleep_for(CD_SLEEP_TIME);
      continue;
    }
    std::pair<Channel, Message> pair = inbox . front ();
#ifdef CD_COMM_LOGGING
    std::cout << CD_TICKTIME << " : Communicator::receive (" << SELF << ") Inbox non-empty (" << inbox.size() << " messages)\n";
    std::cout << CD_TICKTIME << " : Communicator::receive (" << SELF << ") Popping from inbox a message with tag " << pair.second.tag << "\n";
#endif
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

#endif
