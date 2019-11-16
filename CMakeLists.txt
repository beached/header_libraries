cmake_minimum_required( VERSION 3.10 )

project( header_libraries )

find_package( Boost 1.58.0 COMPONENTS system filesystem regex unit_test_framework iostreams REQUIRED )

find_package( Threads )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

add_definitions( -DBOOST_TEST_DYN_LINK -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )

set( HEADER_FOLDER "include" )
set( TEST_FOLDER "tests" )

option( GLEAN_INSTALL_ROOT "" )
if( EXISTS "${GLEAN_INSTALL_ROOT}" ) 
	message( "using glean dependencies" )
	message( "installing to ${GLEAN_INSTALL_ROOT}" )

	include_directories( SYSTEM "${GLEAN_INSTALL_ROOT}/include" )
	link_directories( "${GLEAN_INSTALL_ROOT}/lib" )
endif( )

include_directories( ${HEADER_FOLDER} )

set( TESTED_HEADERS_PREFIXES
	daw_string_view
)

#No boost test
set( TESTED_HEADERS_PREFIXES_NBT
	daw_memory_mapped_file
)

#No boost
set( TESTED_HEADERS_PREFIXES_NB
	cpp_17
	daw_algorithm
	daw_array
	daw_benchmark
	daw_bit
	daw_bit_queues
	daw_bounded_array
 	daw_bounded_graph
	daw_bounded_hash_map
	daw_bounded_hash_set
	daw_bounded_string
	daw_bounded_vector
	daw_carray
	daw_checked_expected
	daw_clumpy_sparsy
	daw_container_algorithm
	daw_copiable_unique_ptr
	daw_cstring
	daw_cxmath
	daw_endian
	daw_exception
	daw_expected
	daw_fixed_lookup
	daw_fnv1a_hash
	daw_function
	daw_function_table
	daw_generic_hash
	daw_graph
	daw_graph_algorithm
	daw_hash_set
	#	daw_hash_table
	daw_heap_array
	daw_heap_value
	daw_keep_n
	daw_math
	daw_natural
	daw_overload
	daw_optional
	daw_optional_poly
	daw_ordered_map
	daw_parse_to
	daw_parser_helper
	daw_parser_helper_sv
	daw_piecewise_factory
	daw_poly_value
	daw_poly_var
	daw_poly_vector
	daw_random
	daw_read_file
	daw_read_only
	daw_safe_string
	daw_scope_guard
	daw_sip_hash
	daw_size_literals
	daw_sort_n
	daw_stack_function
#	daw_static_bitset
	daw_string
	daw_string_fmt
	daw_string_split_range
	daw_span
	daw_traits
	daw_tuple_helper
	daw_uninitialized_storage
	daw_union_pair
	daw_unique_array
	daw_utility
	daw_validated
	daw_value_ptr
	daw_variant_cast
	daw_view
	daw_virtual_base
	daw_visit
	not_null
)


set( UNTESTED_HEADER_IMPL
	daw_int_to_iterator
	daw_math_impl
	daw_string_impl
	daw_traits_concepts
	daw_traits_impl
)

set( TESTED_PARALLEL_HEADERS_PREFIXES
	daw_observable_ptr_pair
)

set( TESTED_PARALLEL_HEADERS_PREFIXES_NB
	daw_copy_mutex
	daw_counter
	daw_latch
	daw_locked_value
	daw_observable_ptr
	daw_scoped_multilock
	daw_semaphore
	daw_spin_lock
)

set( TESTED_ITERATOR_HEADERS_PREFIXES
	daw_circular_iterator
)

set( TESTED_ITERATOR_HEADERS_PREFIXES_NB
	daw_argument_iterator
	daw_back_inserter
	daw_checked_iterator_proxy
	daw_indexed_iterator
	daw_inserter
	daw_end_inserter
	daw_integer_iterator
	daw_output_stream_iterator
	daw_random_iterator
	daw_repeat_n_char_iterator
	daw_reverse_iterator
	daw_sorted_insert_iterator
#	daw_split_iterator
#	daw_zipiter
	InputIterator
)

#Still testing and may not work
set( DEV_HEADER_FILES
	daw_hash_table2
	daw_range
	daw_range_algorithm
	#	daw_virtual_holder
)

set( DEV_HEADER_FILES_NB
	daw_range_collection
)

#set( DEV_ITERATOR_HEADER_FILES
#)

set( UNTESTED_HEADER_FILES
	boost_test.h
	daw_bit_stream.h
	daw_common_mixins.h
	daw_do_n.h
	daw_enable_if.h
	daw_function.h
	daw_move.h
	daw_newhelper.h
	daw_operators.h
	daw_parser_addons.h
	daw_range_common.h
	daw_range_operators.h
	daw_range_reference.h
	daw_range_type.h
	daw_reference.h
	daw_string_view_fwd.h
	daw_swap.h
	daw_zipcontainer.h
)

set( UNTESTED_ITERATOR_HEADER_FILES
	daw_iterator.h
	daw_function_iterator.h
)
set( UNTESTED_PARALLEL_HEADER_FILES
	daw_atomic_unique_ptr.h
	concurrent_queue.h
	daw_condition_variable.h
	daw_locked_stack.h
	daw_shared_mutex.h
	daw_unique_mutex.h
)

enable_testing( )

include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

#set( CMAKE_CTEST_COMMAND ctest -V )
add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )
add_custom_target( full )
add_dependencies( full test_main )

add_library( test_main EXCLUDE_FROM_ALL ${TEST_FOLDER}/test_main.cpp )
add_dependencies( check test_main )
add_dependencies( check full )

#Full Boost
foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

#No boost test
foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES_NBT} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

#No boost
foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	#	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_ITERATOR_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h" DESTINATION include/daw/iterator )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h ${TEST_FOLDER}/iterator/${CUR_PREFIX}_test.cpp )
	#	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_ITERATOR_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h" DESTINATION include/daw/iterator )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h ${TEST_FOLDER}/iterator/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_PARALLEL_HEADERS_PREFIXES} )
	install( FILES "${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h" DESTINATION include/daw/parallel )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h ${TEST_FOLDER}/parallel/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_PARALLEL_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h" DESTINATION include/daw/parallel )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h ${TEST_FOLDER}/parallel/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${DEV_HEADER_FILES} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin test_main ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${DEV_HEADER_FILES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${UNTESTED_HEADER_IMPL} )
	install( FILES "${HEADER_FOLDER}/daw/impl/${CUR_PREFIX}.h" DESTINATION include/daw/impl )
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

