/*
 *  Communicator.hpp
 */

 //          Copyright Shaun Harker 2011.
 // Distributed under the Boost Software License, Version 1.0.
 //    (See accompanying file LICENSE_1_0.txt or copy at
 //          http://www.boost.org/LICENSE_1_0.txt)
 

#include "mpi.h"
#include <string>

// Inlined Functions

inline Communicator::Communicator ( void ) {
} /* Communicator::Communicator */

inline Communicator::~Communicator ( void ) {
} /* Communicator::~Communicator */

inline void Communicator::initialize ( void ) {
	/* Determine identity. */
  int comm_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
	/* Initialize member variables. */
  SELF . channel = comm_rank;
  DIRECTOR . channel = 0;
	ANYTAG = MPI_ANY_TAG;
  ANYSOURCE . channel = MPI_ANY_SOURCE;
}

inline void Communicator::finalize ( void ) {}

inline void Communicator::send ( const Message & send_me, const Channel & target ) {
	std::string message = send_me . str ();
  if ( message . size () > buffer_length ) {
    std::cout << "cluster-delegator: BUFFER NOT LARGE ENOUGH!\n";
    std::cout << "Trying to send a message of size " << message . size () << "\n";
    abort ();
  }
	MPI_Send ( const_cast<char *> ( message  . data () ), message . length (),
            MPI_CHAR, target . channel, send_me . tag, MPI_COMM_WORLD );
} /* Communicator::send */

inline void Communicator::receive ( Message * receive_me, 
                                    Channel * sender, int tag, const Channel & source ) {
  /* Receive the message */
	MPI_Status status;
  char * buffer = new char [ buffer_length ];
	MPI_Recv ( buffer, buffer_length, MPI_CHAR, source . channel, tag, MPI_COMM_WORLD, &status );
	/* Get the length of the message */
	int count;
	MPI_Get_count ( &status, MPI_CHAR, &count );
	/* Produce a string holding the message */
	std::string message ( buffer, count );
	/* Copy the message into the Message structure */
	receive_me -> str ( message );
	receive_me -> tag = status . MPI_TAG;
  /* Identify the source of the message */
	sender -> channel = status . MPI_SOURCE;
  delete [] buffer;
} /* Communicator::receive */

inline bool Communicator::probe ( int tag ) {
	int flag; 
	MPI_Status status;
	MPI_Iprobe ( MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &flag, &status );
	return flag ? true : false;
} /* Communicator::probe */

inline void Communicator::broadcast ( const Message & send_me ) {
  std::string message = send_me . str ();
  int numtasks;
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  for ( int i = 1; i < numtasks; ++ i ) {
    Channel target;
    target . channel = i;
    send ( send_me, target );
  }
}

inline bool Communicator::coordinating ( void ) {
  if ( SELF . channel == DIRECTOR . channel ) return true;
  return false;
}
