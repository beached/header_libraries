cmake_minimum_required( VERSION 3.1 )

project( header_libraries )

find_package( Boost 1.58.0 COMPONENTS system filesystem regex unit_test_framework iostreams REQUIRED )

find_package( Threads )

set( CMAKE_CXX_STANDARD 14 CACHE STRING "The C++ standard whose features are requested.")
add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_definitions( -DNOMINMAX -DD_WIN32_WINNT=0x0601 )
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options( -march=native -pthread -Weverything -Wno-c++98-compat -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion -Wno-missing-braces)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-march=native -pthread -Wall -Wno-deprecated-declarations -Wduplicated-cond -Wlogical-op -Wnull-dereference -Wold-style-cast -Wshadow)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")
	endif( )
endif( )

set( HEADER_FOLDER "include" )
set( TEST_FOLDER "tests" )

include_directories( ${HEADER_FOLDER} )

set( TESTED_HEADERS_PREFIXES
	cpp_17
	daw_algorithm
	daw_array
	daw_array_view
	daw_benchmark
	daw_bit
	daw_bit_queues
	daw_bounded_stack
	daw_carray
	daw_checked_expected
	daw_clumpy_sparsy
	daw_container_algorithm
	daw_copiable_unique_ptr
	daw_cstring
	daw_endian
	daw_exception
	daw_expected
	daw_fixed_lookup
	daw_fixed_stack
	daw_fnv1a_hash
	daw_generic_hash
	daw_hash_table
	daw_heap_array
	daw_heap_value
	daw_math
	daw_memory_mapped_file
	daw_natural
	daw_optional
	daw_optional_poly
	daw_overload
	daw_parse_to
	daw_parser_helper
	daw_parser_helper_sv
	daw_poly_vector
	daw_random
	daw_read_file
	daw_read_only
	daw_safe_string
	daw_scope_guard
	daw_size_literals
	daw_span
	daw_static_array
	daw_static_optional
	daw_static_string
	daw_string
	daw_string_fmt
	daw_string_split_range
	daw_string_view
	daw_traits
	daw_tuple_helper
	daw_unique_array
	daw_union_pair
	daw_utility
	daw_validated
	daw_value_ptr
	not_null
)

set( TESTED_PARALLEL_HEADERS_PREFIXES
	daw_copy_mutex
	daw_latch
	daw_locked_value
	daw_observable_ptr
	daw_observable_ptr_pair
	daw_scoped_multilock
	daw_semaphore
	daw_spin_lock
)

set( TESTED_ITERATOR_HEADERS_PREFIXES
	daw_argument_iterator
	daw_checked_iterator_proxy
	daw_circular_iterator
	daw_integer_iterator
	daw_output_stream_iterator
	daw_random_iterator
	daw_reverse_iterator
	daw_zipiter
)

#Still testing and may not work
set( DEV_HEADER_FILES
	daw_hash_table2
	daw_range
	daw_range_algorithm
	daw_range_collection
	#daw_variant3
	#	daw_virtual_holder
)

#set( DEV_ITERATOR_HEADER_FILES
#	daw_split_iterator
#)

set( UNTESTED_HEADER_FILES
	boost_test.h
	daw_bit_stream.h
	daw_common_mixins.h
	daw_function.h
	daw_newhelper.h
	daw_operators.h
	daw_parser_addons.h
	daw_range_common.h
	daw_range_operators.h
	daw_range_reference.h
	daw_range_type.h
	daw_reference.h
	daw_string_impl.h
	daw_variant.h
	daw_variant2.h
	daw_zipcontainer.h
)

set( UNTESTED_ITERATOR_HEADER_FILES
	daw_iterator.h
	daw_function_iterator.h
)
set( UNTESTED_PARALLEL_HEADER_FILES
	concurrent_queue.h
	daw_condition_variable.h
	daw_locked_stack.h
)

enable_testing( )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

#set( CMAKE_CTEST_COMMAND ctest -V )
add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )

add_library( test_main EXCLUDE_FROM_ALL ${TEST_FOLDER}/test_main.cpp )
add_dependencies( check test_main )

foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( check ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_ITERATOR_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h" DESTINATION include/daw/iterator )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h ${TEST_FOLDER}/iterator/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( check ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_PARALLEL_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h" DESTINATION include/daw/parallel )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h ${TEST_FOLDER}/parallel/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( check ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${DEV_HEADER_FILES} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_dependencies( check ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${UNTESTED_HEADER_FILES} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}" DESTINATION include/daw )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${UNTESTED_ITERATOR_HEADER_FILES} )
	install( FILES "${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}" DESTINATION include/daw/iterator )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${UNTESTED_PARALLEL_HEADER_FILES} )
	install( FILES "${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}" DESTINATION include/daw/parallel )
endforeach( CUR_PREFIX )

