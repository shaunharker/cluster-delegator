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

    //if ( flag ) std::cout << "RECEIVE FLAG!";
    // Check if there is a message to send
    send_mtx . lock ();
    if ( not outbox . empty () ) action = 2; // send (overrides receive)
    send_mtx . unlock ();

    //if ( action > 0 ) std::cout << "ACTION = " << action << "\n";
    // Dispatch on chosen action
    switch ( action ) {
      case 0: // sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        break;
      case 1: // receive
      {
        //std::cout << "Communicator::daemon receive\n";

        /* Get the length of the message */
        int buffer_length;
        int error_code = MPI_Get_count(&status, MPI_CHAR, &buffer_length);
        if ( error_code != 0 ) throw std::runtime_error("Communicator::receive Problem with message size");
        int tag = status . MPI_TAG;
        Channel channel = status . MPI_SOURCE;  
        receive_mtx . lock ();
        std::string & text = incoming [ channel ];
        if ( buffer_length == 0 ) { 
          /* Edge case: message is integer multiple of GB size */
          MPI_Recv ( NULL, 0, MPI_CHAR, channel, tag, MPI_COMM_WORLD, &status );
          Message message;
          message . str ( text );
          message . tag = tag;
          inbox . push ( std::make_pair ( channel, message ) );
          text . clear ();
        } else {
          /* Allocate space for the message */
          uint64_t N = text . size ();
          text . resize ( N + (uint64_t) buffer_length );
          char * buffer = &text[N];
          /* Receive the message */
          MPI_Recv ( buffer, buffer_length, MPI_CHAR, channel, tag, MPI_COMM_WORLD, &status );
        }
        if ( buffer_length < MAX_MESSAGE_SIZE ) { 
          Message message;
          message . str ( text );
          message . tag = tag;
          inbox . push ( std::make_pair ( channel, message ) );
          text . clear ();
        }
        receive_mtx . unlock ();
        //std::cout << "Communicator::daemon receive complete\n";
        break;
      }
      case 2: // send message
      {
        //std::cout << "Communicator::daemon send begin\n";
        // Retrieve the message
        send_mtx . lock ();
        std::pair<Channel, Message> pair = outbox . top ();
        outbox . pop ();
        send_mtx . unlock ();
        Channel channel = pair . first;
        std::string text = pair . second . str ();
        int tag = pair . second . tag;
        uint64_t begin = 0;
        uint64_t N = text . size ();
        bool edge_case = false;
        if ( N == 0 ) edge_case = true;
        while ( N > 0 ) {
          int submessage_length = std::min ( (uint64_t) MAX_MESSAGE_SIZE, N );
          MPI_Send ( const_cast<char *> ( text . data () + begin ), 
                     submessage_length,
                     MPI_CHAR, 
                     channel, 
                     tag, 
                     MPI_COMM_WORLD );
          begin += submessage_length;
          N -= submessage_length;
          if ( submessage_length == MAX_MESSAGE_SIZE && N == 0 ) { 
            edge_case = true; // Message is integer GB in length
          }
        }
        /* Edge case must send addition zero-length message */
        if ( edge_case ) {   
          MPI_Send ( NULL,
                     0,
                     MPI_CHAR, 
                     channel, 
                     tag, 
                     MPI_COMM_WORLD ); 
        }
        //std::cout << "Communicator::daemon send complete\n";
        break;
      }
    }
  }
  //std::cout << "daemon exit!\n";
}

inline void Communicator::
send ( const Message & message, 
       const Channel & channel ) {
  send_mtx . lock ();
  outbox . push ( std::make_pair ( channel, message ) );
  send_mtx . unlock ();
}

inline void Communicator::
receive ( Message * message, 
          Channel * channel ) {
  while ( 1 ) {
    receive_mtx . lock ();
    if ( inbox . empty () ) { 
      receive_mtx . unlock ();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    std::pair<Channel, Message> pair = inbox . top ();
    inbox . pop ();
    receive_mtx . unlock ();
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

