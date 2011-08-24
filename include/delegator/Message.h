/*
 *  Message.h
 */

//          Copyright Shaun Harker 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef _CMDP_MESSAGE_
#define _CMDP_MESSAGE_

#include <sstream>
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"

//let's include a few serialization files so the user won't have to understand this
#include "boost/serialization/vector.hpp"
#include "boost/serialization/deque.hpp"
#include "boost/serialization/set.hpp"
#include "boost/serialization/map.hpp"
#include "boost/serialization/list.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/utility.hpp"
#include "boost/serialization/hash_set.hpp"
#include "boost/serialization/hash_map.hpp"
#include "boost/serialization/unordered_set.hpp"
#include "boost/serialization/unordered_map.hpp"

class Message {
 
  enum state_type { READING, WRITING, CLOSED };
  mutable state_type state;
  mutable boost::archive::binary_oarchive * output_archive;
  mutable boost::archive::binary_iarchive * input_archive;
  mutable std::stringbuf data;
 
  /** open_for_reading ().
   *    instructs the message object that it is about to be read, 
   *    so it can make necessary preparations.
   */
  void open_for_reading ( void ) const;
 
  /** open_for_writing ().
   *    instructs the message object that it is about to be written, 
   *    so it can make necessary preparations.
   */
  void open_for_writing ( void );
 
  /** close ().
   *    instructs the message object that it is about to be closed, 
   *    so it can finalize either the reading or writing process.
   */
  void close ( void ) const;
 
public:
 
  int tag;
 
  /** Constructor */
  Message ( void );
 
  /** Message Copy Constructor. */
  Message ( const Message & copy_me );
 
  /** Message Deconstructor */
  ~Message ( void );
 
  /** std::string str ().  
   *   Returns a std::string object (which is not null-terminated) containing 
   *   the serialized contents of the message.
   */
  std::string str ( void ) const;
  
  /** void str ( const std::string copy_contents ); 
   *   Sets the serialized contents of the message to be whatever is contained in 
   *   the string "copy_contents"
   */
  void str ( const std::string copy_contents );
 
 
  /** << 
   *    Streaming operator is overloaded to store data structures into a message.
   *    The right hand side must implement "serialize" in a manner compatiable with
   *    the Boost serialization library. 
   */
  template < class T > 
  Message & operator << ( const T & insert_me );
 
  /** >> 
   *    Streaming operator is overloaded to retrieve data structures from a message.
   *    The right hand side must implement "serialize" in a manner compatiable with
   *    the Boost serialization library. 
   */
  template < class T > 
  const Message & operator >> ( T & extract_me ) const;
  
  // assignment  (warning, this closes the messages)
  Message & operator = ( Message & rhs );

};

#include "delegator/Message.hpp"

#endif
