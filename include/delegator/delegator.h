/// delegator.h
/// Shaun Harker 
/// 2011


#ifndef SRH_DELEGATOR_H
#define SRH_DELEGATOR_H

#include "delegator/Message.h"
#include "delegator/Communicator.h"
#include "delegator/Coordinator_Worker_Scheme.h"

/***************************************
 *         USER INTERFACE              *
 ***************************************/

namespace delegator {
// Simplest interface: call start, then run, then stop.
  /** Start */
  void Start ( void );
  
  /** Run */
  template < class Process >
  int Run ( void );

  /** Stop */
  void Stop ( void );
  
// or, you can call run and pass command line arguments:
  /** Run */
  template < class Process >
  int Run ( int argc, char * argv [] );
  
// or, if you develop other schemes and communicators, you could
// use this final invokation. Otherwise, ignore it:
  /** Run                                                      *
   *     More advanced interface which can specify different  *
   *     Scheme or Communicator                               */
  template < class Process, class Scheme, class Communicator >
  int Run ( int argc, char * argv [] );
}

#include "delegator/delegator.hpp"

#endif
