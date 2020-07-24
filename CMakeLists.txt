cmake_minimum_required( VERSION 3.10 )

project( header_libraries )

find_package( Threads )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

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

# These tests crash MSVC
set( NOTWIN_TESTED_HEADERS_PREFIXES_NB
 	daw_bounded_graph
	daw_bounded_hash_map
	daw_bounded_hash_set
	daw_parser_helper
	daw_piecewise_factory
)

set( TESTED_HEADERS_PREFIXES_NB
	cpp_17
	daw_algorithm
	daw_array
	daw_benchmark
	daw_bind_args_at
	daw_bit
	daw_bit_queues
	daw_bounded_array
	daw_bounded_string
	daw_bounded_vector
	daw_carray
	daw_checked_expected
	daw_clumpy_sparsy
	daw_container_algorithm
	daw_copiable_unique_ptr
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
	daw_heap_array
	daw_heap_value
	daw_keep_n
	daw_math
	daw_memory_mapped_file
	daw_metro_hash
	daw_natural
	daw_overload
	daw_optional
	daw_optional_poly
	daw_ordered_map
	daw_parse_to
	daw_parser_helper_sv
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
	daw_string_split_range
	daw_string_view
	daw_span
	daw_traits
	daw_tuple_helper
	daw_uint64_buffer
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

set( TESTED_PARALLEL_HEADERS_PREFIXES_NB
	daw_copy_mutex
	daw_counter
	daw_latch
	daw_locked_value
	daw_scoped_multilock
	daw_semaphore
)

set( DEV_PARALLEL_HEADERS_PREFIXES_NB
	daw_observable_ptr
	daw_observable_ptr_pair
	daw_spin_lock
)

set( TESTED_ITERATOR_HEADERS_PREFIXES_NB
	daw_argument_iterator
	daw_back_inserter
	daw_checked_iterator_proxy
	daw_circular_iterator
	daw_counting_iterators
	daw_indexed_iterator
	daw_inserter
	daw_end_inserter
	daw_integer_iterator
	daw_output_stream_iterator
	daw_repeat_n_char_iterator
	daw_reverse_iterator
	daw_sorted_insert_iterator
#	daw_split_iterator
#	daw_zipiter
	InputIterator
)

set( DEV_HEADER_FILES_NB
	daw_cstring
	daw_hash_table2
	daw_range
	daw_min_perfect_hash
	daw_stack_quick_sort
	daw_range_algorithm
	daw_range_collection
)

#set( DEV_ITERATOR_HEADER_FILES
#)

set( UNTESTED_HEADER_FILES
	daw_bit_stream.h
	daw_cfile.h
	daw_common_mixins.h
	daw_do_n.h
	daw_do_not_optimize.h
	daw_enable_if.h
	daw_exchange.h
	daw_function.h
	daw_hash_table.h
	daw_hide.h
	daw_move.h
	daw_newhelper.h
	daw_operators.h
	daw_parser_addons.h
	daw_range_common.h
	daw_range_operators.h
	daw_range_reference.h
	daw_range_type.h
	daw_reference.h
	daw_string_fmt.h
	daw_string_view_fwd.h
	daw_swap.h
	daw_zipcontainer.h
)

set( UNTESTED_ITERATOR_HEADER_FILES
	daw_iterator.h
	daw_function_iterator.h
	daw_random_iterator.h
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

#set( CMAKE_CTEST_COMMAND ctest -V )
add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )
add_custom_target( full )

foreach( CUR_PREFIX ${NOTWIN_TESTED_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	if( MSVC )
		message( "Excluding ${CUR_PREFIX} from testing" )
	else( ) 
		add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
		add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
		add_dependencies( full ${CUR_PREFIX}_test_bin )
	endif( )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_ITERATOR_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h" DESTINATION include/daw/iterator )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/iterator/${CUR_PREFIX}.h ${TEST_FOLDER}/iterator/${CUR_PREFIX}_test.cpp )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${TESTED_PARALLEL_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h" DESTINATION include/daw/parallel )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h ${TEST_FOLDER}/parallel/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test ${CUR_PREFIX}_test_bin )
	add_dependencies( full ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${DEV_PARALLEL_HEADERS_PREFIXES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h" DESTINATION include/daw/parallel )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/parallel/${CUR_PREFIX}.h ${TEST_FOLDER}/parallel/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
endforeach( CUR_PREFIX )

foreach( CUR_PREFIX ${DEV_HEADER_FILES_NB} )
	install( FILES "${HEADER_FOLDER}/daw/${CUR_PREFIX}.h" DESTINATION include/daw )
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${HEADER_FOLDER}/daw/${CUR_PREFIX}.h ${TEST_FOLDER}/${CUR_PREFIX}_test.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
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

