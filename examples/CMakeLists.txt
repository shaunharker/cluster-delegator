
set ( LIBS ${LIBS} ${Boost_LIBRARIES} ${MPI_LIBRARIES} )

add_executable ( helloworld helloworld.cpp )
target_link_libraries ( helloworld ${LIBS} )

add_executable ( skeleton skeleton.cpp )
target_link_libraries ( skeleton ${LIBS} )

add_executable ( example1 example1.cpp )
target_link_libraries ( example1 ${LIBS} cluster-delegator )

add_executable ( example2 example2.cpp )
target_link_libraries ( example2 ${LIBS} )

if(MPI_COMPILE_FLAGS)
  set_target_properties(helloworld PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(skeleton PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(example1 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(example2 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(helloworld PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(skeleton PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(example1 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(example2 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(TARGETS helloworld skeleton example1 example2 
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin )
