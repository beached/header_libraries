// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/take.h"

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

	/// The size of the range is known in O(1) and matches the number of
	/// elements found by walking it
	template<typename R>
	void ensure_sized( R &&r, std::ptrdiff_t expected ) {
		static_assert(
		  std::sized_sentinel_for<decltype( std::end( r ) ),
		                          decltype( std::begin( r ) )>,
		  "the end of a take_view must be a sized sentinel" );
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

	DAW_ATTRIB_NOINLINE void test_take_is_sized( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		ensure_sized( pipeline( v, Take( 3 ) ), 3 );
		ensure_sized( pipeline( v, Take( 0 ) ), 0 );
		// Taking more than there is stops at the end of the range
		ensure_sized( pipeline( v, Take( 100 ) ), 8 );
	}

	DAW_ATTRIB_NOINLINE void test_take_of_unbounded_iota_is_sized( ) {
		ensure_sized( pipeline( iota_view<std::size_t>{ 0, daw::max_value<std::size_t> },
		                        Take( 3 ) ),
		              3 );
	}

	DAW_ATTRIB_NOINLINE void test_take_after_other_views_is_sized( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		ensure_sized( pipeline( v,
		                        Map( []( int x ) {
			                        return x * 2;
		                        } ),
		                        Take( 5 ) ),
		              5 );
		ensure_sized( pipeline( v, Reverse, Take( 3 ) ), 3 );
	}

	DAW_ATTRIB_NOINLINE void test_take_distance_after_increment( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 };
		auto r = pipeline( v, Take( 4 ) );
		auto it = std::begin( r );
		++it;
		daw_ensure( std::end( r ) - it == 3 );
		it += 3;
		daw_ensure( std::end( r ) - it == 0 );
		daw_ensure( it == std::end( r ) );
	}

	// std::list knows its size but cannot subtract iterators, so the size comes
	// from size( )
	DAW_ATTRIB_NOINLINE void test_take_size_over_sized_bidirectional_range( ) {
		for( std::size_t n = 0; n < 10; ++n ) {
			auto l = std::list<int>( );
			for( std::size_t i = 0; i < n; ++i ) {
				l.push_back( static_cast<int>( i ) );
			}
			for( std::size_t how_many : { 0U, 1U, 3U, 9U, 20U } ) {
				auto r = pipeline( l, Take( how_many ) );
				static_assert( std::ranges::sized_range<decltype( r )> );
				auto walked = std::size_t{ 0 };
				for( auto it = std::begin( r ); it != std::end( r ); ++it ) {
					++walked;
				}
				daw_ensure( r.size( ) == std::min( n, how_many ) );
				daw_ensure( walked == r.size( ) );
			}
		}
	}

	DAW_ATTRIB_NOINLINE void test_take_has_no_size_over_unsized_range( ) {
		auto fl = std::forward_list<int>{ 1, 2, 3 };
		auto r = pipeline( fl, Take( 2 ) );
		static_assert( not has_size_member<decltype( r )> );
	}

	// std::ranges::filter_view only has a non-const begin( )
	DAW_ATTRIB_NOINLINE void test_take_of_filter_view( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6 };
		auto even = []( int x ) {
			return x % 2 == 0;
		};
		auto fv = v | std::views::filter( even );
		daw_ensure( ( pipeline( fv, Take( 2 ), To<std::vector> ) ==
		              std::vector<int>{ 2, 4 } ) );
		daw_ensure( ( pipeline( v | std::views::filter( even ), Take( 2 ),
		                        To<std::vector> ) == std::vector<int>{ 2, 4 } ) );
	}
} // namespace tests

int main( ) {
	tests::test_take_is_sized( );
	tests::test_take_of_unbounded_iota_is_sized( );
	tests::test_take_after_other_views_is_sized( );
	tests::test_take_distance_after_increment( );
	tests::test_take_size_over_sized_bidirectional_range( );
	tests::test_take_has_no_size_over_unsized_range( );
	tests::test_take_of_filter_view( );
}
