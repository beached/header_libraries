// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/to.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <cstddef>
#include <map>
#include <ranges>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	// A random access range whose end is a different type than its begin
	DAW_ATTRIB_NOINLINE void test_to_vector_from_random_non_common_range( ) {
		auto const v = To<std::vector>( )( std::views::iota( 0 ) |
		                                    std::views::take( 10 ) );
		daw_ensure( v.size( ) == 10 );
		daw_ensure( v.front( ) == 0 and v.back( ) == 9 );
	}

	// A forward only range whose end is a different type than its begin
	DAW_ATTRIB_NOINLINE void test_to_vector_from_forward_non_common_range( ) {
		auto src = std::vector<int>{ 1, 2, 3, 4, 5 };
		auto const v =
		  To<std::vector>( )( src | std::views::take_while( []( int x ) {
			                      return x < 4;
		                      } ) );
		daw_ensure( ( v == std::vector<int>{ 1, 2, 3 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_to_vector_from_take( ) {
		auto src = std::vector<int>{ 1, 2, 3, 4, 5 };
		auto const v = pipeline( src, Take( 3 ), To<std::vector> );
		static_assert( std::is_same_v<decltype( v ), std::vector<int> const> );
		daw_ensure( ( v == std::vector<int>{ 1, 2, 3 } ) );

		auto const all = pipeline( src, Take( 100 ), To<std::vector> );
		daw_ensure( all == src );

		auto const none = pipeline( src, Take( 0 ), To<std::vector> );
		daw_ensure( none.empty( ) );
	}

	DAW_ATTRIB_NOINLINE void test_to_vector_from_zip( ) {
		auto a = std::vector<int>{ 1, 2, 3, 4 };
		auto b = std::vector<int>{ 5, 6, 7 };
		auto const v = pipeline( zip_view( a, b ), To<std::vector> );
		daw_ensure( v.size( ) == 3 );
		daw_ensure( v[2].first == 3 and v[2].second == 7 );
	}

	DAW_ATTRIB_NOINLINE void test_to_vector_from_enumerate( ) {
		auto a = std::vector<int>{ 10, 20, 30 };
		auto const v = pipeline( a, Enumerate, To<std::vector> );
		daw_ensure( v.size( ) == 3 );
		daw_ensure( v[1].first == 1 and v[1].second == 20 );
	}

	DAW_ATTRIB_NOINLINE void test_to_vector_from_elements_of_zip( ) {
		auto a = std::vector<int>{ 10, 20, 30 };
		auto b = std::vector<int>{ 1, 2, 3 };
		auto z = Zip( a, b );
		auto const v = To<std::vector>( )( Elements<1>( z ) );
		daw_ensure( v.size( ) == 3 );
		daw_ensure( std::get<0>( v[2] ) == 3 );
	}

	// A two range zip produces pairs, so the map type is deduced from the
	// iterators
	DAW_ATTRIB_NOINLINE void test_to_map_from_zip( ) {
		auto keys = std::vector<int>{ 1, 2, 3 };
		auto values = std::vector<int>{ 10, 20, 30 };
		auto const m = pipeline( zip_view( keys, values ), To<std::map> );
		static_assert( std::is_same_v<decltype( m ), std::map<int, int> const> );
		daw_ensure( m.size( ) == 3 );
		daw_ensure( m.at( 2 ) == 20 );
	}

	// istream_view only has a non-const begin( ), as begin( ) reads the first
	// value, and its iterator is move only.  It is still a Range, and To builds
	// the container by inserting each element
	DAW_ATTRIB_NOINLINE void test_to_vector_from_istream_view( ) {
		auto ss = std::istringstream( "1 2 3" );
		auto const v = To<std::vector>( )( std::views::istream<int>( ss ) );
		static_assert( std::is_same_v<decltype( v ), std::vector<int> const> );
		daw_ensure( ( v == std::vector<int>{ 1, 2, 3 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_to_vector_from_istream_view_lvalue( ) {
		auto ss = std::istringstream( "4 5" );
		auto iv = std::views::istream<int>( ss );
		auto const v = To<std::vector>( )( iv );
		static_assert( std::is_same_v<decltype( v ), std::vector<int> const> );
		daw_ensure( ( v == std::vector<int>{ 4, 5 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_to_vector_from_empty_istream_view( ) {
		auto ss = std::istringstream( "" );
		auto const v = To<std::vector>( )( std::views::istream<int>( ss ) );
		static_assert( std::is_same_v<decltype( v ), std::vector<int> const> );
		daw_ensure( v.empty( ) );
	}
} // namespace tests

int main( ) {
	tests::test_to_vector_from_random_non_common_range( );
	tests::test_to_vector_from_forward_non_common_range( );
	tests::test_to_vector_from_take( );
	tests::test_to_vector_from_zip( );
	tests::test_to_vector_from_enumerate( );
	tests::test_to_vector_from_elements_of_zip( );
	tests::test_to_map_from_zip( );
	tests::test_to_vector_from_istream_view( );
	tests::test_to_vector_from_istream_view_lvalue( );
	tests::test_to_vector_from_empty_istream_view( );
}
