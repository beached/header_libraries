// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/skip.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <algorithm>
#include <cstddef>
#include <forward_list>
#include <iterator>
#include <list>
#include <ranges>
#include <vector>

namespace tests {
	using namespace daw::pipelines;

	/// True when R has a size( ) member.  The check needs a template context to
	/// be false rather than an error
	template<typename R>
	inline constexpr bool has_size_member =
	  requires( daw::remove_cvref_t<R> const &r ) { r.size( ); };

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

	template<typename Container>
	void check_skip_size( ) {
		for( std::size_t n = 0; n < 10; ++n ) {
			auto c = Container( );
			for( std::size_t i = 0; i < n; ++i ) {
				c.push_back( static_cast<int>( i ) );
			}
			for( std::size_t how_many : { 0U, 1U, 3U, 9U, 20U } ) {
				// Skipping more than there is leaves an empty range
				ensure_size_matches( pipeline( c, Skip( how_many ) ),
				                     n - std::min( n, how_many ) );
			}
		}
	}

	DAW_ATTRIB_NOINLINE void test_skip_size_over_random_range( ) {
		check_skip_size<std::vector<int>>( );
	}

	// std::list knows its size but cannot subtract iterators
	DAW_ATTRIB_NOINLINE void test_skip_size_over_sized_bidirectional_range( ) {
		check_skip_size<std::list<int>>( );
	}

	// Only a range that knows its size without walking gives Skip a size( )
	DAW_ATTRIB_NOINLINE void test_skip_has_no_size_over_unsized_range( ) {
		auto fl = std::forward_list<int>{ 1, 2, 3 };
		auto r = pipeline( fl, Skip( 1 ) );
		static_assert( not has_size_member<decltype( r )> );
	}

	// Repeated calls to begin( ) give the same position, and the const begin( )
	// agrees with it
	DAW_ATTRIB_NOINLINE void test_skip_begin_is_stable( ) {
		auto l = std::list<int>{ 1, 2, 3, 4, 5 };
		auto r = pipeline( l, Skip( 2 ) );
		auto const &cr = r;
		daw_ensure( *std::begin( r ) == 3 );
		daw_ensure( std::begin( r ) == std::begin( r ) );
		daw_ensure( *std::begin( cr ) == 3 );
		daw_ensure( &*std::begin( r ) == &*std::begin( cr ) );
	}

	// A copy of a view that owns its range refers to its own range, not the
	// original's, so it still works after the original is gone
	DAW_ATTRIB_NOINLINE void test_skip_copy_of_owning_view( ) {
		auto r = pipeline( std::list<int>{ 1, 2, 3, 4, 5 }, Skip( 2 ) );
		auto const &cr = r;
		daw_ensure( *std::begin( r ) == 3 );
		daw_ensure( *std::begin( cr ) == 3 );

		auto copy = r;
		auto const &ccopy = copy;
		daw_ensure( &*std::begin( copy ) != &*std::begin( r ) );
		daw_ensure( &*std::begin( ccopy ) != &*std::begin( cr ) );
		*std::begin( copy ) = 100;
		daw_ensure( *std::begin( copy ) == 100 );
		daw_ensure( *std::begin( r ) == 3 );

		auto walked = std::vector<int>{ };
		for( auto it = std::begin( copy ); it != std::end( copy ); ++it ) {
			walked.push_back( *it );
		}
		daw_ensure( ( walked == std::vector<int>{ 100, 4, 5 } ) );
	}

	// std::ranges::filter_view is only iterable when not const
	DAW_ATTRIB_NOINLINE void test_skip_of_filter_view( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6 };
		auto fv = v | std::views::filter( []( int x ) {
			          return x % 2 == 0;
		          } );
		auto r = pipeline( fv, Skip( 1 ) );
		auto walked = std::vector<int>{ };
		for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
			walked.push_back( *it );
		}
		daw_ensure( ( walked == std::vector<int>{ 4, 6 } ) );
	}
} // namespace tests

int main( ) {
	tests::test_skip_size_over_random_range( );
	tests::test_skip_size_over_sized_bidirectional_range( );
	tests::test_skip_has_no_size_over_unsized_range( );
	tests::test_skip_begin_is_stable( );
	tests::test_skip_copy_of_owning_view( );
	tests::test_skip_of_filter_view( );
}
