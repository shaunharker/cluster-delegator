/// Message.h
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_MESSAGE_H
#define CLUSTER_DELEGATOR_MESSAGE_H

#include <sstream>
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"

// Let's include a few serialization files
// That way the user will only have to worry about
// this if they use their own complex data types
#include "boost/serialization/vector.hpp"
#include "boost/serialization/deque.hpp"
#include "boost/serialization/set.hpp"
#include "boost/serialization/map.hpp"
#include "boost/serialization/list.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/utility.hpp"
#include "boost/serialization/unordered_set.hpp"
#include "boost/serialization/unordered_map.hpp"

class Message {
 
  enum state_type { READING, WRITING, CLOSED };
  mutable state_type state;
  mutable boost::archive::binary_oarchive * output_archive;
  mutable boost::archive::binary_iarchive * input_archive;
  mutable std::stringbuf data;
 
  /// open_for_reading ().
  ///    instructs the message object that it is about to be read, 
  ///    so it can make necessary preparations.
  void 
  open_for_reading ( void ) const;
 
  /// open_for_writing ().
  ///    instructs the message object that it is about to be written, 
  ///    so it can make necessary preparations.
  void 
  open_for_writing ( void );
 
  /// close ().
  ///    instructs the message object that it is about to be closed, 
  ///    so it can finalize either the reading or writing process.
  void 
  close ( void ) const;
 
public:
 
  /// tag
  ///   tag associated with MPI message
  int tag;
 
  /// Message
  ///   default constructor
  Message ( void ); 

  /// Message
  ///   copy constructor
  Message ( Message const& copy_me );

  /// ~Message
  ///   deconstructor
  ~Message ( void );
 
  /// str 
  ///   Returns a std::string object (which is not null-terminated) containing 
  ///   the serialized contents of the message.
  std::string 
  str ( void ) const;
  
  /// str
  ///   Sets the serialized contents of the message to be whatever is contained in 
  ///   the string "copy_contents"
  void 
  str ( std::string const& copy_contents );
 
 
  /// operator << 
  ///    Streaming operator is overloaded to store data structures into a message.
  ///    The right hand side must implement "serialize" in a manner compatiable with
  ///    the Boost serialization library. 
  template < class T > Message & 
  operator << ( T const& insert_me );
 
  /// operator >> 
  ///    Streaming operator is overloaded to retrieve data structures from a message.
  ///    The right hand side must implement "serialize" in a manner compatiable with
  ///    the Boost serialization library. 
  template < class T > Message const& 
  operator >> ( T & extract_me ) const;
  
  /// operator =
  ///  Assignment operator
  ///  Warning: this closes the messages.
  Message & 
  operator = ( Message & rhs );

};

#include "delegator/Message.hpp"

#endif
