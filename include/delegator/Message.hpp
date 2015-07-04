/// Message.hpp
/// Shaun Harker 
/// 2011

// Templated Functions
template < class T >
Message & Message::
operator << ( T const& insert_me ) {
  if ( state != WRITING ) open_for_writing ();
  *output_archive << insert_me;
  return *this;
}
 
template < class T >
const Message & Message::
operator >> ( T & extract_me ) const {
  if ( state != READING ) open_for_reading ();
  *input_archive >> extract_me;
  return *this;
}

// Inlined Functions
inline Message::
Message ( void ) {
  state = CLOSED;
}

inline Message::
Message ( Message const& copy_me ) {
  state = CLOSED;
  str ( copy_me . str () );
  tag = copy_me . tag;
}

inline Message::
~Message ( void ) {
  close ();
}

inline std::string Message::
str ( void ) const {
  close ();
  return data . str ();
}

inline void Message::
str ( std::string const& copy_contents ) {
  close ();
  data . str ( copy_contents );
}

inline void Message::
open_for_reading ( void ) const {
  close ();
  input_archive = new boost::archive::binary_iarchive ( data );
  state = READING;
}

inline void Message::
open_for_writing ( void ) {
  close ();
  output_archive = new boost::archive::binary_oarchive ( data );
  state = WRITING;
}

inline void Message::
close ( void ) const {
  if ( state == READING ) delete input_archive;
  if ( state == WRITING ) delete output_archive;
  state = CLOSED;
}

inline Message & Message::
operator = ( Message & rhs ) {
  state = rhs . state;
  output_archive = rhs . output_archive;
  input_archive = rhs . input_archive;
  tag = rhs . tag;
  str ( rhs . str () );
  return * this;
}
