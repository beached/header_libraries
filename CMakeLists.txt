cmake_minimum_required( VERSION 2.8.0 )

project( header_libraries )

find_package( Boost 1.59.0 COMPONENTS system filesystem regex unit_test_framework REQUIRED )
set( Boost_LIBS -lboost_system -lboost_filesystem -lboost_regex -lboost_unit_test_framework )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
	set( Boost_LIBS )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion -Wno-global-constructors" )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package( Threads REQUIRED )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations" )
endif( )

if( THREADS_HAVE_PTHREAD_ARG )
	set( COMPILER_SPECIFIC_LIBS, "-lpthread" )
endif( )

set( TESTED_HEADERS_PREFIXES
	daw_algorithm
	daw_range_algorithm
	daw_range
	daw_traits
	daw_utility
 )

set( UNTESTED_HEADER_FILES
	concurrent_queue.h
	daw_array.h
	daw_common_mixins.h
	daw_exception.h
	daw_expected.h
	daw_parallel_algorithm.h
	daw_range_collection.h
	daw_range_common.h
	daw_range_operators.h
	daw_range_reference.h
	daw_range_type.h
	daw_reference.h
	daw_string.h
	daw_zipcontainer.h
	daw_zipiter.h
	make_unique.h
	scope_guard.h
)

add_definitions( -DBOOST_TEST_DYN_LINK ) 
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES} )
	install( FILES "${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin ${CUR_PREFIX}.h ${CUR_PREFIX}_test.cpp header_libraries_test/test_main.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBS} ${COMPILER_SPECIFIC_LIBS} )
	add_test( ${CUR_PREFIX}_test, ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${UNTESTED_HEADER_FILES} )
	install( FILES "${CUR_PREFIX}" DESTINATION include/daw )
endforeach( CUR_PREFIX )


