cmake_minimum_required(VERSION 2.8.0)

project(header_libraries)

set(CLANG_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -Weverything -Wno-c++98-compat  -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes")
set(GCC_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O3 -Wall" )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	message( STATUS "Using Clang" )
	set( CMAKE_CXX_FLAGS "${CLANG_CMAKE_CXX_FLAGS}" )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	message( STATUS "Using GNU" )
	set( CMAKE_CXX_FLAGS "${GCC_CMAKE_CXX_FLAGS}" )
else( )
	message( STATUS "Using Other Compiler -> ${CMAKE_CXX_COMPILER_ID}" )
endif( )

find_package(Boost 1.55.0 COMPONENTS system filesystem iostreams unit_test_framework REQUIRED)

set(HEADER_PREFIXES
		algorithm
		concurrent_queue
		daw_common_mixins
		daw_range
		daw_string
		daw_traits
		daw_utility
		exception
		expected
		make_unique
		range_algorithm
		scope_guard
)

enable_testing( )

#add_definitions( -DBOOST_TEST_MAIN )
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 

foreach( CUR_PREFIX ${HEADER_PREFIXES} )
	add_executable( ${CUR_PREFIX}_test_bin ${CUR_PREFIX}.h ${CUR_PREFIX}_test.cpp header_libraries_test/test_main.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBRARIES} )
	install( FILES ${CUR_PREFIX}.h DESTINATION include/header_libaries )
	add_test( ${CUR_PREFIX}_test, ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )



