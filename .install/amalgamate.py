#!/usr/bin/python
"""Amalgate delegator-cpp library sources into a single source and header file.
Requires Python 2.6
Example of invocation (must be invoked from delegator-cpp top directory):
python amalgate.py
"""
import os
import os.path
import sys

class AmalgamationFile:
    def __init__( self, top_dir ):
        self.top_dir = top_dir
        self.blocks = []

    def add_text( self, text ):
        if not text.endswith( '\n' ):
            text += '\n'
        self.blocks.append( text )

    def add_file( self, relative_input_path, wrap_in_comment=False ):
        def add_marker( prefix ):
            self.add_text( '' )
            self.add_text( '// ' + '/'*70 )
            self.add_text( '// %s of content of file: %s' % (prefix, relative_input_path.replace('\\','/')) )
            self.add_text( '// ' + '/'*70 )
            self.add_text( '' )
        add_marker( 'Beginning' )
        f = open( os.path.join( self.top_dir, relative_input_path ), 'rt' )
        content = f.read()
        if wrap_in_comment:
            content = '/*\n' + content + '\n*/'
        self.add_text( content )
        f.close()
        add_marker( 'End' )
        self.add_text( '\n\n\n\n' )

    def get_value( self ):
        return ''.join( self.blocks ).replace('\r\n','\n')

    def write_to( self, output_path ):
        output_dir = os.path.dirname( output_path )
        if output_dir and not os.path.isdir( output_dir ):
            os.makedirs( output_dir )
        f = open( output_path, 'wb' )
        f.write( self.get_value() )
        f.close()

def amalgamate_source( source_top_dir=None,
                       target_source_path=None,
                       header_include_path=None ):
    """Produces amalgated source.
       Parameters:
           source_top_dir: top-directory
           target_source_path: output .cpp path
           header_include_path: generated header path relative to target_source_path.
    """
    print 'Amalgating header...'
    header = AmalgamationFile( source_top_dir )
    header.add_text( '/// cluster-delegator amalgated header' )
    header.add_text( '/// It is intended to be used with #include <%s>' % header_include_path )
    header.add_file( 'LICENSE', wrap_in_comment=True )
    header.add_text( '#ifndef CLUSTERDELEGATOR_H_INCLUDED' )
    header.add_text( '# define CLUSTERDELEGATOR_H_INCLUDED' )
    header.add_text( '/// If defined, indicates that the source file is amalgated' )
    header.add_text( '/// to prevent private header inclusion.' )
    header.add_text( '#define CLUSTER_DELEGATOR_IS_AMALGAMATION' )
    header.add_file( 'include/delegator/Message.h' )
    header.add_file( 'include/delegator/Message.hpp' )
    header.add_file( 'include/delegator/Communicator.h' )
    header.add_file( 'include/delegator/Communicator.hpp' )
    header.add_file( 'include/delegator/Coordinator_Worker_Process.h' )
    header.add_file( 'include/delegator/Coordinator_Worker_Process.hpp' )
    header.add_file( 'include/delegator/Coordinator_Worker_Scheme.h' )
    header.add_file( 'include/delegator/Coordinator_Worker_Scheme.hpp' )
    header.add_file( 'include/delegator/delegator.h' )
    header.add_file( 'include/delegator/delegator.hpp' )

    header.add_text( '#endif //ifndef CLUSTERDELEGATOR_H_INCLUDED' )

    target_header_path = os.path.join( os.path.dirname(target_source_path), header_include_path )
    print 'Writing amalgated header to %r' % target_header_path
    header.write_to( target_header_path )

def main():
    usage = """%prog [options]
Generate a single amalgated source and header file from the sources.
"""
    from optparse import OptionParser
    parser = OptionParser(usage=usage)
    parser.allow_interspersed_args = False
    parser.add_option('-s', '--source', dest="target_source_path", action='store', default='dist/cluster-delegator.cpp',
        help="""Output .cpp source path. [Default: %default]""")
    parser.add_option('-i', '--include', dest="header_include_path", action='store', default='cluster-delegator.hpp',
        help="""Header include path. Used to include the header from the amalgated source file. [Default: %default]""")
    parser.add_option('-t', '--top-dir', dest="top_dir", action='store', default=os.getcwd(),
        help="""Source top-directory. [Default: %default]""")
    parser.enable_interspersed_args()
    options, args = parser.parse_args()

    msg = amalgamate_source( source_top_dir=options.top_dir,
                             target_source_path=options.target_source_path,
                             header_include_path=options.header_include_path )
    if msg:
        sys.stderr.write( msg + '\n' )
        sys.exit( 1 )
    else:
        print 'Source succesfully amalagated'
 
if __name__ == '__main__':
    main()
