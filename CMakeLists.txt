cmake_minimum_required(VERSION 2.8.0)

project(header_libraries)


find_package(Boost 1.59.0 COMPONENTS system filesystem regex unit_test_framework REQUIRED)
set(Boost_LIBS -lboost_system -lboost_filesystem -lboost_regex -lboost_unit_test_framework)

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601" )
	set( Boost_LIBS )
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
	find_package(Threads REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated-declarations")
ENDIF( )

set(HEADER_PREFIXES
		concurrent_queue
		daw_algorithm
		daw_common_mixins
		daw_exception
		daw_expected		
		daw_parallel_algorithm
		daw_range	
		daw_string
		daw_traits
		daw_utility
		make_unique
		range_algorithm
		scope_guard
		
)

enable_testing( )

#add_definitions( -DBOOST_TEST_MAIN )
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 
include_directories( SYSTEM ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

foreach( CUR_PREFIX ${HEADER_PREFIXES} )
	add_executable( ${CUR_PREFIX}_test_bin ${CUR_PREFIX}.h ${CUR_PREFIX}_test.cpp header_libraries_test/test_main.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBS} )
	add_test( ${CUR_PREFIX}_test, ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

