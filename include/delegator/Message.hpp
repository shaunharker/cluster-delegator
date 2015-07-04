/// Message.hpp
/// Shaun Harker 
/// 2011

#ifndef CLUSTER_DELEGATOR_MESSAGE_HPP
#define CLUSTER_DELEGATOR_MESSAGE_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#ifndef CLUSTER_DELEGATOR_IS_AMALGAMATION
#include "Message.h"
#endif

#include <string>
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"

// Inlined Functions
INLINE_IF_HEADER_ONLY Message::
Message ( void ) {
  state = CLOSED;
}

INLINE_IF_HEADER_ONLY Message::
Message ( Message const& copy_me ) {
  state = CLOSED;
  str ( copy_me . str () );
  tag = copy_me . tag;
}

INLINE_IF_HEADER_ONLY Message::
~Message ( void ) {
  close ();
}

INLINE_IF_HEADER_ONLY std::string Message::
str ( void ) const {
  close ();
  return data . str ();
}

INLINE_IF_HEADER_ONLY void Message::
str ( std::string const& copy_contents ) {
  close ();
  data . str ( copy_contents );
}

INLINE_IF_HEADER_ONLY void Message::
open_for_reading ( void ) const {
  close ();
  input_archive = new boost::archive::binary_iarchive ( data );
  state = READING;
}

INLINE_IF_HEADER_ONLY void Message::
open_for_writing ( void ) {
  close ();
  output_archive = new boost::archive::binary_oarchive ( data );
  state = WRITING;
}

INLINE_IF_HEADER_ONLY void Message::
close ( void ) const {
  if ( state == READING ) delete input_archive;
  if ( state == WRITING ) delete output_archive;
  state = CLOSED;
}

INLINE_IF_HEADER_ONLY Message & Message::
operator = ( Message & rhs ) {
  state = rhs . state;
  output_archive = rhs . output_archive;
  input_archive = rhs . input_archive;
  tag = rhs . tag;
  str ( rhs . str () );
  return * this;
}

#endif
