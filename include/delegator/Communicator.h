/// Communicator.h
/// Shaun Harker 
/// 2011

#ifndef _CMDP_COMMUNICATOR_
#define _CMDP_COMMUNICATOR_

#include "delegator/Message.h"
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <stack>
#include <queue>
#include <utility>

/** Channel. Used by Communicator. */
typedef int Channel;

/** Communicator */
class Communicator {
public:
  Channel SELF;
  Channel DIRECTOR;
  Channel ANYSOURCE;
  int ANYTAG;
  
  /** daemon
        call on main thread. Run-loop which handles MPI. */
  void daemon ( void );

  /** initialize ( void );
   - Responsible for initializing communications. 
   - Responsible for setting the values of DIRECTOR, SELF, ANYSOURCE, ANYTAG. */
  void initialize ( void );
  
  /** finalize ( void ).
   *      - Responsible for finalizing communications. */ 
  void finalize ( void );
  
  /** send ( const Message & send_me, const Channel & target ). **
   *    sends the contents of send_me to "target"               */
  void send ( const Message & send_me, const Channel & target );
 
  /** void receive ( Message * receive_me, Channel * sender ).
   *   Receives a message and stores it in "receive_me".
   *   Identity of sender is stored in "sender"
   */
  void receive ( Message * receive_me, Channel * sender ); 
 
  /** bool coordinating ( void );                       **
   *    Checks to see if this is a "coordinating" node. */
  bool coordinating ( void );

  /** halt
   *   Stop daemon */
  void halt ( void );

private:
  std::mutex mtx;
  std::atomic<bool> daemon_on;
  std::unordered_map<Channel, std::string> incoming;
  std::queue<std::pair<Channel, Message>> inbox;
  std::queue<std::pair<Channel, Message>> outbox;

};

#include "delegator/Communicator.hpp"

#endif
