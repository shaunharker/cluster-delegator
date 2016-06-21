# add the executable

set ( LIBS      ${LIBS}
                ${Boost_LIBRARIES}
                ${MPI_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT} )

add_library ( libcluster-delegator STATIC cluster-delegator.cpp )
set_target_properties( libcluster-delegator PROPERTIES OUTPUT_NAME cluster-delegator )

add_library ( libcluster-delegatorshared SHARED cluster-delegator.cpp )
set_target_properties( libcluster-delegatorshared PROPERTIES OUTPUT_NAME cluster-delegator )
target_link_libraries ( libcluster-delegatorshared ${LIBS} )

install(TARGETS libcluster-delegator libcluster-delegatorshared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
