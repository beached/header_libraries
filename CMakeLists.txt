cmake_minimum_required( VERSION 2.8.12 )

project( header_libraries )

enable_testing( )

find_package( Boost 1.58.0 COMPONENTS system filesystem regex unit_test_framework iostreams REQUIRED )

find_package( Threads )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -stdlib=libc++ -ferror-limit=1 -Weverything -Wno-c++98-compat -Wfatal-errors -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)		
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -Wall -Werror -fmax-errors=1 -Wno-deprecated-declarations)
	endif()
endif()

set( TESTED_HEADERS_PREFIXES
	daw_algorithm
	daw_clumpy_sparsy
	daw_hash_table
	daw_math
	daw_memory
	daw_memory_mapped_file
	daw_range_algorithm
	daw_range
	daw_traits
	daw_utility
 )

set( UNTESTED_HEADER_FILES
	concurrent_queue.h
	daw_array.h
	daw_bit.h
	daw_bit_queues.h
	daw_common_mixins.h
	daw_cstring.h
	daw_exception.h
	daw_expected.h
	daw_newhelper.h
	daw_optional.h
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
	add_executable( ${CUR_PREFIX}_test_bin EXCLUDE_FROM_ALL ${CUR_PREFIX}.h ${CUR_PREFIX}_test.cpp header_libraries_test/test_main.cpp )
	target_link_libraries( ${CUR_PREFIX}_test_bin ${Boost_LIBRARIES} ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
	add_test( ${CUR_PREFIX}_test, ${CUR_PREFIX}_test_bin )
	list( APPEND test_bins ${CUR_PREFIX}_test_bin )
endforeach( CUR_PREFIX )

add_custom_target( tests )
add_dependencies( tests ${test_bins} )


foreach( CUR_PREFIX ${UNTESTED_HEADER_FILES} )
	install( FILES "${CUR_PREFIX}" DESTINATION include/daw )
endforeach( CUR_PREFIX )

