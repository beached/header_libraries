#Copyright( c ) Darrell Wright
#
#Distributed under the Boost Software License, Version 1.0.( See accompanying
#file LICENSE or copy at http: // www.boost.org/LICENSE_1_0.txt)
#
#Official repository : https: // github.com/beached/header_libraries
#
cmake_policy( SET CMP0065 NEW )

set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

if( CMAKE_BUILD_TYPE EQUAL "Debug" )
    add_compile_definitions( DEBUG=1 )
endif()


set( TEST_SOURCES
     InputIterator_test.cpp
     cpp_17_test.cpp
     daw_algorithm_test.cpp
     daw_arith_traits_test.cpp
     daw_array_test.cpp
     daw_benchmark_test.cpp
     daw_bounded_array_test.cpp
     daw_bounded_string_test.cpp
     daw_bounded_vector_test.cpp
     daw_carray_test.cpp
     daw_checked_expected_test.cpp
     daw_clumpy_sparsy_test.cpp
     daw_constant_test.cpp
     daw_container_algorithm_test.cpp
     daw_copiable_unique_ptr_test.cpp
     daw_cx_offset_of_test.cpp
     daw_cxmath_test.cpp
     daw_endian_test.cpp
     daw_exception_test.cpp
     daw_expected_test.cpp
     daw_fixed_array_test.cpp
     daw_fixed_lookup_test.cpp
     daw_fnv1a_hash_test.cpp
     daw_function_table_test.cpp
     daw_function_ref_test.cpp
     daw_function_test.cpp
     daw_function_view_test.cpp
     daw_fwd_pack_apply_test.cpp
     daw_generic_hash_test.cpp
     daw_graph_algorithm_test.cpp
     daw_graph_test.cpp
     daw_hash_set_test.cpp
     daw_heap_array_test.cpp
     daw_heap_value_test.cpp
     daw_integers_signed_test.cpp
     daw_iterator_argument_iterator_test.cpp
     daw_iterator_back_inserter_test.cpp
     daw_iterator_checked_iterator_proxy_test.cpp
     daw_iterator_chunk_iterator_test.cpp
     daw_iterator_circular_iterator_test.cpp
     daw_iterator_counting_iterators_test.cpp
     daw_iterator_end_inserter_test.cpp
     daw_iterator_find_iterator_test.cpp
     daw_iterator_indexed_iterator_test.cpp
     daw_iterator_inserter_test.cpp
     daw_iterator_integer_iterator_test.cpp
     daw_iterator_iota_iterator_test.cpp
     daw_iterator_output_stream_iterator_test.cpp
     daw_iterator_random_iterator_test.cpp
     daw_iterator_repeat_n_char_iterator_test.cpp
     daw_iterator_reverse_iterator_test.cpp
     daw_iterator_sorted_insert_iterator_test.cpp
     daw_iterator_zipiter_test.cpp
     daw_keep_n_test.cpp
     daw_lift_test.cpp
     daw_logic_test.cpp
     daw_math_test.cpp
     daw_memory_mapped_file_test.cpp
     daw_metro_hash_test.cpp
     daw_natural_test.cpp
     daw_not_null_test.cpp
     daw_nth_pack_element_test.cpp
     daw_optional_poly_test.cpp
     daw_optional_test.cpp
     daw_ordered_map_test.cpp
     daw_overload_test.cpp
     daw_parse_to_test.cpp
     daw_parser_helper_sv_test.cpp
     daw_poly_value_test.cpp
     daw_poly_var_test.cpp
     daw_poly_vector_test.cpp
     daw_prop_const_ptr_test.cpp
     daw_random_test.cpp
     daw_read_file_test.cpp
     daw_read_only_test.cpp
     daw_ref_counted_pointer_test.cpp
     daw_ring_adaptor_test.cpp
     daw_safe_string_test.cpp
     daw_scope_guard_test.cpp
     daw_simple_array_test.cpp
     daw_sip_hash_test.cpp
     daw_size_literals_test.cpp
     daw_span_test.cpp
     daw_stack_function_test.cpp
     daw_string_concat_test.cpp
     daw_string_split_range_test.cpp
     daw_string_test.cpp
     daw_string_view1_test.cpp
     daw_string_view2_test.cpp
     daw_take_test.cpp
     daw_traits_test.cpp
     daw_tuple_helper_test.cpp
     daw_tuple_test.cpp
     daw_uint_buffer_test.cpp
     daw_uninitialized_storage_test.cpp
     daw_union_pair_test.cpp
     daw_unique_array_test.cpp
     daw_unique_ptr_test.cpp
     daw_utility_test.cpp
     daw_validated_test.cpp
     daw_value_ptr_test.cpp
     daw_variant_cast_test.cpp
     daw_view_test.cpp
     daw_virtual_base_test.cpp
     daw_visit_test.cpp
     daw_zipcontainer_test.cpp
     sbo_test.cpp
     static_hash_table_test.cpp
)

set( CPP20_TEST_SOURCES
     daw_concepts_test.cpp
     daw_contiguous_view_test.cpp
     daw_iter_view_test.cpp
     daw_named_params_test.cpp
     vector_test.cpp
)
#NOT COMPLETED daw_iterator_split_iterator_test.cpp
#NOT COMPLETED daw_static_bitset_test.cpp
#NOT COMPLETED daw_string_fmt_test.cpp

set( NOT_MSVC_TEST_SOURCES
     daw_can_constant_evaluate_test.cpp
     daw_bounded_graph_test.cpp
     daw_bounded_hash_map_test.cpp
     daw_bounded_hash_set_test.cpp
     daw_container_help_test.cpp
     daw_parser_helper_test.cpp
     daw_piecewise_factory_test.cpp
     daw_tuple2_test.cpp
)

#not included in CI as they are not ready
set( DEV_TEST_SOURCES
     daw_bind_args_at_test.cpp
     daw_bit_queues_test.cpp
     daw_bit_test.cpp
     daw_cstring_test.cpp
     daw_hash_table2_test.cpp
     daw_min_perfect_hash_test.cpp
     daw_range_algorithm_test.cpp
     daw_range_collection_test.cpp
     daw_range_test.cpp
     daw_sort_n_test.cpp
     daw_stack_quick_sort_test.cpp
     daw_tracked_allocator_test.cpp
     daw_vector_test.cpp
)

include( cmake/test_compiler_options.cmake )

find_package( Threads REQUIRED )

#Allows building all in some IDE's
add_custom_target( ${PROJECT_NAME}_full )

add_library( daw_test INTERFACE )
target_link_libraries( daw_test INTERFACE daw::header_libraries ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
target_compile_options( daw_test INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/permissive-> )

set( all_tests ${TEST_SOURCES} )

if( NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    list( APPEND all_tests ${NOT_MSVC_TEST_SOURCES} )
endif()

foreach( CUR_TEST IN LISTS all_tests )
    string( REPLACE ".cpp" "" CUR_TEST_NAME ${CUR_TEST} )
    add_executable( "${CUR_TEST_NAME}" "${CUR_TEST}" )
    target_link_libraries( ${CUR_TEST_NAME} PRIVATE daw_test )
    add_test( ${CUR_TEST_NAME}_test ${CUR_TEST_NAME} )
    add_dependencies( ${PROJECT_NAME}_full ${CUR_TEST_NAME} )
endforeach()

if( CMAKE_CXX_STANDARD STRGREATER_EQUAL "20" )
    foreach( CUR_TEST IN LISTS CPP20_TEST_SOURCES )
        string( REPLACE ".cpp" "" CUR_TEST_NAME ${CUR_TEST} )
        add_executable( "${CUR_TEST_NAME}" "${CUR_TEST}" )
        target_link_libraries( ${CUR_TEST_NAME} PRIVATE daw_test )
        target_compile_features( ${CUR_TEST_NAME} PUBLIC cxx_std_20 )
        add_test( ${CUR_TEST_NAME}_test ${CUR_TEST_NAME} )
        add_dependencies( ${PROJECT_NAME}_full ${CUR_TEST_NAME} )
    endforeach()
else()
    message( STATUS "Disabling C++20 tests" )
endif()

option( DAW_ENABLE_DEV_TESTING "Build unit tests still in development" OFF )
foreach( CUR_TEST IN LISTS DEV_TEST_SOURCES )
    string( REPLACE ".cpp" "" CUR_TEST_NAME ${CUR_TEST} )
    if( DAW_ENABLE_DEV_TESTING )
        add_executable( ${CUR_TEST_NAME} ${CUR_TEST} )
        add_dependencies( ${PROJECT_NAME}_full ${CUR_TEST_NAME} )
    else()
        add_executable( ${CUR_TEST_NAME} EXCLUDE_FROM_ALL ${CUR_TEST} )
    endif()
    target_link_libraries( ${CUR_TEST_NAME} PRIVATE daw_test )
endforeach()
