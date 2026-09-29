// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/flatten.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <iterator>
#include <list>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace tests {
	using namespace daw::pipelines;
	using nested_t = std::vector<std::vector<int>>;

	template<typename R>
	std::vector<int> collect( R &&r ) {
		auto result = std::vector<int>{ };
		for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
			result.push_back( *it );
		}
		return result;
	}

	// Leading and inner empty ranges are skipped
	DAW_ATTRIB_NOINLINE void test_flatten_values( ) {
		auto n = nested_t{ { }, { }, { 1, 2 }, { }, { 3 } };
		auto r = pipeline( n, Flatten );
		daw_ensure( ( collect( r ) == std::vector<int>{ 1, 2, 3 } ) );
		// Walking it again gives the same result
		daw_ensure( ( collect( r ) == std::vector<int>{ 1, 2, 3 } ) );
	}

	// Repeated calls to begin( ) give the same position, and the const begin( )
	// agrees with it
	DAW_ATTRIB_NOINLINE void test_flatten_begin_is_stable( ) {
		auto n = std::list<std::vector<int>>{ { }, { 4, 5 }, { 6 } };
		auto r = pipeline( n, Flatten );
		auto const &cr = r;
		daw_ensure( *std::begin( r ) == 4 );
		daw_ensure( std::begin( r ) == std::begin( r ) );
		// The first element comes from the first non-empty inner range
		daw_ensure( &*std::begin( r ) == &( *std::next( n.begin( ) ) )[0] );
		daw_ensure( &*std::begin( r ) == &*std::begin( cr ) );
	}

	// Over a const outer range the inner ranges are const too
	DAW_ATTRIB_NOINLINE void test_flatten_const_view( ) {
		auto n = nested_t{ { }, { 1, 2 }, { }, { 3 } };
		auto const r = pipeline( n, Flatten );
		static_assert( std::is_same_v<decltype( *std::begin( r ) ), int const &> );
		daw_ensure( ( collect( r ) == std::vector<int>{ 1, 2, 3 } ) );

		auto const &cn = n;
		auto rc = pipeline( cn, Flatten );
		daw_ensure( ( collect( rc ) == std::vector<int>{ 1, 2, 3 } ) );
	}

	// A copy of a view that owns its range refers to its own range, not the
	// original's, so it still works after the original is gone
	DAW_ATTRIB_NOINLINE void test_flatten_copy_of_owning_view( ) {
		auto copy = std::optional<decltype( pipeline( nested_t{ }, Flatten ) )>{ };
		{
			auto original = pipeline( nested_t{ { }, { 1, 2 }, { 3 } }, Flatten );
			daw_ensure( *std::begin( original ) == 1 );
			copy.emplace( original );
			daw_ensure( &*std::begin( *copy ) != &*std::begin( original ) );
		}
		daw_ensure( ( collect( *copy ) == std::vector<int>{ 1, 2, 3 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_flatten_move_of_owning_view( ) {
		auto original = pipeline( nested_t{ { }, { 1, 2 }, { 3 } }, Flatten );
		daw_ensure( *std::begin( original ) == 1 );
		auto moved = std::move( original );
		daw_ensure( ( collect( moved ) == std::vector<int>{ 1, 2, 3 } ) );
	}

	DAW_ATTRIB_NOINLINE void test_flatten_all_empty( ) {
		auto n = nested_t{ { }, { } };
		auto r = pipeline( n, Flatten );
		daw_ensure( std::begin( r ) == std::end( r ) );
	}
} // namespace tests

int main( ) {
	tests::test_flatten_values( );
	tests::test_flatten_begin_is_stable( );
	tests::test_flatten_const_view( );
	tests::test_flatten_copy_of_owning_view( );
	tests::test_flatten_move_of_owning_view( );
	tests::test_flatten_all_empty( );
}
