cmake_minimum_required( VERSION 2.8.12 )

project( header_libraries )

find_package( Boost 1.58.0 COMPONENTS system filesystem regex unit_test_framework iostreams REQUIRED )

find_package( Threads )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 /std:c++latest ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -g -Weverything -Wno-c++98-compat -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)		
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -g -Wall -Wno-deprecated-declarations)
	endif()
endif()

set( HEADER_FOLDER "include" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( TESTED_HEADERS_PREFIXES
	daw_algorithm
	daw_array
	daw_bit_queues
	daw_bounded_stack
	daw_benchmark
	daw_checked_iterator_proxy
	daw_clumpy_sparsy
	daw_cstring
	daw_expected
	daw_hash_table
	daw_heap_value
	daw_math
	daw_memory_address
	daw_memory_mapped_file
	daw_optional
	daw_optional_poly
	daw_parser_helper
	daw_poly_vector
	daw_range_algorithm
	daw_range
	daw_safe_string
	daw_traits
	daw_tuple_helper
	daw_utility
	daw_variant
 )

set( UNTESTED_HEADER_FILES
	concurrent_queue.h
	daw_bit.h
	daw_bit_stream.h
	daw_common_mixins.h
	daw_exception.h
	daw_fnv1a_hash.h
	daw_locked_stack.h
	daw_newhelper.h
	daw_operators.h
	daw_parallel_algorithm.h
	daw_parser_addons.h
	daw_range_collection.h
	daw_range_common.h
	daw_range_operators.h
	daw_range_reference.h
	daw_range_type.h
	daw_read_only.h
	daw_reference.h
	daw_semaphore.h
	daw_string.h
	daw_zipcontainer.h
	daw_zipiter.h
	cpp_17.h
	scope_guard.h
)

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK ) 
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp ${TEST_FOLDER}/test_main.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test, ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${UNTESTED_HEADER_FILES} )
	install( FILES "${HEADER_FOLDER}/${CUR_PREFIX}" DESTINATION include/daw )
endforeach( CUR_PREFIX )

