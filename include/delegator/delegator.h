/*
 *  delegator.h
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef SRH_DELEGATOR_H
#define SRH_DELEGATOR_H

#include "delegator/Message.h"
#include "delegator/Communicator.h"
#include "delegator/Coordinator_Worker_Scheme.h"

#include <stack>

/***************************************
 *         USER INTERFACE              *
 ***************************************/

namespace delegator {
  /** Start                                         *
   * Simple User Interface hard-wired to            *
   * Coordinator-Worker Scheme and MPI Communicator */
  template < class Process >
  int Start ( void );

  /** Start                                         *
   * Simple User Interface hard-wired to            *
   * Coordinator-Worker Scheme and MPI Communicator */
  template < class Process >
  int Start ( int argc, char * argv [] );

  /** Run Delegator                                           *
   *     More advanced interface which can specify different  *
   *     Scheme or Communicator                               */
  template < class Process, class Scheme, class Communicator >
  int RunDelegator ( int argc, char * argv [] );
}

#include "delegator/delegator.hpp"

#endif
