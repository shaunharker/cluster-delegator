/*
 *  Communicator.h
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef _CMDP_COMMUNICATOR_
#define _CMDP_COMMUNICATOR_

#include "delegator/Message.h"

/** Channel. Used by Communicator. */
class Channel {
public:
  int channel;
  void * data; // for possible extension
  bool operator < ( const Channel & rhs ) const {
    return channel < rhs . channel; 
  }
};

/** Communicator */
class Communicator {
private:
  static const int buffer_length = 128*1024*1024;
public:
  Channel SELF;
  Channel DIRECTOR;
  Channel ANYSOURCE;
  int ANYTAG;
  
  /** Communicator_Base ( void ) */
  Communicator ( void ); 
  
  /** ~Communicator_Base ( void ) */
  virtual ~Communicator ( void );

  /** initialize ( void );
   - Responsible for initializing communications. 
   - Responsible for setting the values of DIRECTOR, SELF, ANYSOURCE, ANYTAG. */
  virtual void initialize ( void );
  
  /** finalize ( void ).
   *      - Responsible for finalizing communications. */ 
  virtual void finalize ( void );
  
  /** send ( const Message & send_me, const Channel & target ). **
   *    sends the contents of send_me to "target"               */
  virtual void send ( const Message & send_me, const Channel & target );
 
  /** void receive ( Message * receive_me, Channel * sender, int tag, const Channel & source ).
   *   Receives a message from "source" with tag "tag" and stores it in "receive_me".
   *   Identity of sender is stored in "sender"
   *   If tag == ANYTAG, message is received with any tag. 
   *   If source == ANYSOURCE, message is received from any source.
   */
  virtual void receive ( Message * receive_me, Channel * sender, int tag, const Channel & source ); 
 
  /** bool probe ( int tag );                                         **
   *    Checks to see if there is an available message with tag "tag" */
  virtual bool probe ( int tag ); 

  /** void broadcast ( const Message & send_me )                      **
   *    Checks to see if there is an available message with tag "tag" */
  virtual void broadcast ( const Message & send_me );
  
  /** bool coordinating ( void );                       **
   *    Checks to see if this is a "coordinating" node. */
  virtual bool coordinating ( void );
};

#include "delegator/Communicator.hpp"

#endif
