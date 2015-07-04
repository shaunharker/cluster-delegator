/// Communicator.h
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_COMMUNICATOR_H
#define CLUSTER_DELEGATOR_COMMUNICATOR_H

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "delegator/Message.h"
#endif

#include <mutex>
#include <atomic>
#include <unordered_map>
#include <queue>
#include <utility>

/// Channel. Used by Communicator.
typedef int Channel;

/// Communicator
///   A class to handle interprocess communication through MPI
class Communicator {
public:
  Channel SELF;
  Channel DIRECTOR;
  Channel ANYSOURCE;
  int ANYTAG;
  
  /// daemon
  ///   call on main thread. Run-loop which handles MPI.
  void 
  daemon ( void );

  /// initialize
  ///   - Responsible for initializing communications.
  ///   - Responsible for setting the values of DIRECTOR, SELF, ANYSOURCE, ANYTAG.
  void 
  initialize ( void );
  
  /// finalize
  ///   - Responsible for finalizing communications. 
  void 
  finalize ( void );
  
  /// send
  ///   sends the contents of send_me to "target"               
  void 
  send ( Message const& send_me, 
         Channel const& target );
 
  /// receive
  ///   Receives a message and stores it in "receive_me".
  ///   Identity of sender is stored in "sender"
  void 
  receive ( Message * receive_me, 
            Channel * sender ); 
 
  /// coordinating ( void );
  ///   Checks to see if this is a "coordinating" node.
  bool 
  coordinating ( void );

  /// halt
  ///   Stop daemon
  void 
  halt ( void );

private:
  std::mutex mtx;
  std::atomic<bool> daemon_on;
  std::unordered_map<Channel, std::string> incoming;
  std::queue<std::pair<Channel, Message>> inbox;
  std::queue<std::pair<Channel, Message>> outbox;
};

#endif
