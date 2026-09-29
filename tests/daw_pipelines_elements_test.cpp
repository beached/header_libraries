// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/elements.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <cstddef>
#include <iterator>
#include <list>
#include <map>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	/// The size of the range is known in O(1) and matches the number of
	/// elements found by walking it
	template<typename R>
	void ensure_sized( R &&r, std::ptrdiff_t expected ) {
		static_assert(
		  std::sized_sentinel_for<decltype( std::end( r ) ),
		                          decltype( std::begin( r ) )>,
		  "the end of an elements view over a counted zip must be a sized "
		  "sentinel" );
		static_assert( std::ranges::sized_range<R> );
		auto const first = std::begin( r );
		auto const last = std::end( r );
		daw_ensure( last - first == expected );
		daw_ensure( first - last == -expected );
		daw_ensure( std::ranges::distance( r ) == expected );
		auto walked = std::ptrdiff_t{ 0 };
		for( auto it = first; it != last; ++it ) {
			++walked;
		}
		daw_ensure( walked == expected );
	}

	DAW_ATTRIB_NOINLINE void test_elements_of_zip_is_sized( ) {
		auto a = std::vector<int>{ 10, 20, 30, 40 };
		auto b = std::vector<int>{ 1, 2, 3 };
		auto z = Zip( a, b );
		ensure_sized( Elements<1>( z ), 3 );
		ensure_sized( Elements<0, 1>( z ), 3 );
		ensure_sized( pipeline( z, Element<0> ), 3 );
	}

	DAW_ATTRIB_NOINLINE void test_elements_of_range_of_pairs_is_sized( ) {
		auto v = std::vector<std::pair<int, int>>{ { 1, 2 }, { 3, 4 }, { 5, 6 } };
		ensure_sized( pipeline( v, Element<0> ), 3 );
		ensure_sized( pipeline( v, Elements<1> ), 3 );
		ensure_sized( pipeline( v, Elements<0, 1> ), 3 );
	}

	// Element and Elements over a random access range are standard random
	// access iterators
	DAW_ATTRIB_NOINLINE void test_elements_are_random_access_iterators( ) {
		auto v = std::vector<std::pair<int, int>>{ { 1, 2 } };
		auto a = std::vector<int>{ 1 };
		auto z = Zip( a, a );
		static_assert( std::random_access_iterator<
		               decltype( std::begin( pipeline( v, Element<0> ) ) )> );
		static_assert( std::random_access_iterator<
		               decltype( std::begin( pipeline( v, Elements<1> ) ) )> );
		static_assert( std::random_access_iterator<
		               decltype( std::begin( pipeline( z, Element<0> ) ) )> );
		static_assert(
		  std::random_access_iterator<decltype( std::begin( Elements<1>( z ) ) )> );
	}

	// operator+( n ) and operator-( n ) return a moved copy and leave the
	// iterator they are called on where it was
	DAW_ATTRIB_NOINLINE void test_element_iterator_plus_minus_n( ) {
		auto a = std::vector<int>{ 10, 20, 30, 40 };
		auto b = std::vector<int>{ 1, 2, 3, 4 };
		auto z = Zip( a, b );
		auto r = pipeline( z, Element<0> );
		auto const first = std::begin( r );
		auto const third = first + 2;
		daw_ensure( *third == 30 );
		daw_ensure( *first == 10 );
		auto const second = third - 1;
		daw_ensure( *second == 20 );
		daw_ensure( *third == 30 );
		daw_ensure( *( 3 + first ) == 40 );
		daw_ensure( third - first == 2 );
	}

	DAW_ATTRIB_NOINLINE void test_elements_iterator_plus_minus_n( ) {
		auto a = std::vector<int>{ 10, 20, 30, 40 };
		auto b = std::vector<int>{ 1, 2, 3, 4 };
		auto z = Zip( a, b );
		auto r = Elements<1>( z );
		auto const first = std::begin( r );
		auto const third = first + 2;
		daw_ensure( std::get<0>( *third ) == 3 );
		daw_ensure( std::get<0>( *first ) == 1 );
		auto const second = third - 1;
		daw_ensure( std::get<0>( *second ) == 2 );
		daw_ensure( std::get<0>( *third ) == 3 );
		daw_ensure( std::get<0>( *( 3 + first ) ) == 4 );
		daw_ensure( third - first == 2 );
	}

	// Element and Elements work over ranges whose reference is a tuple like
	// reference, not only over ranges of tuple values like zip
	DAW_ATTRIB_NOINLINE void test_elements_of_range_of_pairs( ) {
		auto v = std::vector<std::pair<int, int>>{ { 1, 2 }, { 3, 4 }, { 5, 6 } };

		auto keys = pipeline( v, Element<0> );
		auto key_it = std::begin( keys );
		daw_ensure( *key_it == 1 );
		++key_it;
		daw_ensure( *key_it == 3 );

		auto values = pipeline( v, Elements<1> );
		auto value_it = std::begin( values );
		daw_ensure( std::get<0>( *value_it ) == 2 );
		++value_it;
		daw_ensure( std::get<0>( *value_it ) == 4 );
	}

	/// size( ) and std::ranges::size match the number of elements found by
	/// walking the range
	template<typename R>
	void ensure_size_matches( R &&r, std::size_t expected ) {
		static_assert( std::ranges::sized_range<R> );
		daw_ensure( r.size( ) == expected );
		daw_ensure( std::ranges::size( r ) == expected );
		auto walked = std::size_t{ 0 };
		for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
			++walked;
		}
		daw_ensure( walked == expected );
	}

	// std::map and std::list know their size but cannot subtract iterators, so
	// the size comes from size( )
	DAW_ATTRIB_NOINLINE void
	test_elements_size_over_sized_bidirectional_ranges( ) {
		for( std::size_t n = 0; n < 10; ++n ) {
			auto m = std::map<int, int>( );
			auto l = std::list<std::pair<int, int>>( );
			for( std::size_t i = 0; i < n; ++i ) {
				m[static_cast<int>( i )] = static_cast<int>( i * 10 );
				l.emplace_back( static_cast<int>( i ), static_cast<int>( i * 10 ) );
			}
			ensure_size_matches( pipeline( m, Element<0> ), n );
			ensure_size_matches( pipeline( m, Elements<1> ), n );
			ensure_size_matches( pipeline( m, Elements<0, 1> ), n );
			ensure_size_matches( pipeline( l, Element<1> ), n );
			ensure_size_matches( pipeline( l, Elements<0> ), n );
		}
	}

	DAW_ATTRIB_NOINLINE void test_elements_of_map_values( ) {
		auto m = std::map<int, int>{ { 1, 10 }, { 2, 20 }, { 3, 30 } };
		auto keys = pipeline( m, Element<0> );
		auto key_it = std::begin( keys );
		daw_ensure( *key_it == 1 );
		++key_it;
		daw_ensure( *key_it == 2 );
		auto values = pipeline( m, Elements<1> );
		auto value_it = std::begin( values );
		daw_ensure( std::get<0>( *value_it ) == 10 );
	}

	// A temporary range is owned by the view
	DAW_ATTRIB_NOINLINE void test_elements_of_temporary_range( ) {
		auto make = [] {
			return std::vector<std::pair<int, int>>{ { 1, 2 }, { 3, 4 }, { 5, 6 } };
		};
		auto keys = pipeline( make( ), Element<0> );
		ensure_size_matches( keys, 3 );
		daw_ensure( *std::begin( keys ) == 1 );

		auto values = pipeline( make( ), Elements<1> );
		ensure_size_matches( values, 3 );
		daw_ensure( std::get<0>( *std::begin( values ) ) == 2 );
	}
} // namespace tests

int main( ) {
	tests::test_elements_of_zip_is_sized( );
	tests::test_elements_of_range_of_pairs_is_sized( );
	tests::test_elements_are_random_access_iterators( );
	tests::test_element_iterator_plus_minus_n( );
	tests::test_elements_iterator_plus_minus_n( );
	tests::test_elements_of_range_of_pairs( );
	tests::test_elements_size_over_sized_bidirectional_ranges( );
	tests::test_elements_of_map_values( );
	tests::test_elements_of_temporary_range( );
}
