
cmake_minimum_required (VERSION 2.6) 
project (cluster-delegator) 

#########
# rpath #
#########

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

#########
# Flags #
#########

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-depth=2048")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG" )

###############
# Definitions #
###############

add_definitions( -DINSTALLPREFIX="${CMAKE_INSTALL_PREFIX}" )
message( STATUS "Chose the following prefix: " ${CMAKE_INSTALL_PREFIX} )

############
# Packages #
############

find_package(Boost 1.58.0 REQUIRED COMPONENTS serialization)
find_package(MPI REQUIRED)

#######################
# Include Directories #
#######################

include_directories ( 
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/../include
  /usr/local/include 
  /opt/local/include 
  ${MPI_INCLUDE_PATH}
  ${Boost_INCLUDE_DIRS} 
  ${USER_INCLUDE_PATH} )

#######################
# Library Directories #
#######################

link_directories ( 
  ${USER_LIBRARY_PATH} 
  /usr/local/lib 
  /opt/local/lib )

###########
# Outputs #
###########

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#########
# Build #
#########

set ( LIBS ${LIBS} ${Boost_LIBRARIES} ${MPI_LIBRARIES} )

add_executable ( example1 example1.cpp )
target_link_libraries ( example1 ${LIBS} )

add_executable ( example2 example2.cpp )
target_link_libraries ( example2 ${LIBS} )

if(MPI_COMPILE_FLAGS)
  set_target_properties(example1 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(example2 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(example1 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(example2 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(TARGETS example1 example2 
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin )

#########
# Tests #
#########

enable_testing()
add_test( test1 mpiexec -np 2 ${CMAKE_SOURCE_DIR}/build/bin/example1 )
add_test( test2 mpiexec -np 2 ${CMAKE_SOURCE_DIR}/build/bin/example2 42 is the answer )




