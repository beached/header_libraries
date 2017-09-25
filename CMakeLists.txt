cmake_minimum_required( VERSION 2.8.12 )

project( header_libraries )

find_package( Boost 1.58.0 COMPONENTS system filesystem regex unit_test_framework iostreams REQUIRED )

find_package( Threads )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 /std:c++latest ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -pthread -Weverything -Wno-missing-braces -Wno-c++98-compat -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -pthread -Wall -Wno-deprecated-declarations)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	endif( )
endif()

set( HEADER_FOLDER "include" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( TESTED_HEADERS_PREFIXES
	concurrent_queue
	cpp_17
	daw_algorithm
	daw_array
	daw_array_view
	daw_benchmark
	daw_bit
	daw_bit_queues
	daw_bit_stream
	daw_bounded_stack
	daw_carray
	daw_checked_iterator_proxy
	daw_clumpy_sparsy
	daw_common_mixins
	daw_cstring
	daw_exception
	daw_expected
	daw_fixed_stack
	daw_fnv1a_hash
	daw_function_iterator
	daw_hash_table
	daw_heap_array
	daw_heap_value
	daw_locked_stack
	daw_locked_value
	daw_math
	daw_memory_address
	daw_memory_mapped_file
	daw_newhelper
	daw_operators
	daw_optional
	daw_optional_poly
	daw_parser_addons
	daw_parser_helper
	daw_parser_helper_sv
	daw_poly_vector
	daw_random_iterator
	daw_range
	daw_range_algorithm
	daw_range_collection
	daw_range_common
	daw_range_operators
	daw_range_reference
	daw_range_type
	daw_read_only
	daw_reference
	daw_safe_string
	daw_scoped_multilock
	daw_semaphore
	daw_size_literals
	daw_stack_array
	daw_string
	daw_string_view
	daw_traits
	daw_tuple_helper
	daw_utility
	daw_variant
	daw_zipcontainer
	daw_zipiter
	not_null
	scope_guard
)

set( UNTESTED_HEADER_FILES
	boost_test.h
	)

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK ) 
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp ${TEST_FOLDER}/test_main.cpp )
	#add_executable( ${CUR_PREFIX}_test_bin ${HEADER_FOLDER}/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp ${TEST_FOLDER}/test_main.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test, ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${UNTESTED_HEADER_FILES} )
	install( FILES "${HEADER_FOLDER}/${CUR_PREFIX}" DESTINATION include/daw )
endforeach( CUR_PREFIX )

