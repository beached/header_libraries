// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/zip.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

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
		  "the end of a counted zip_view must be a sized sentinel" );
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

	DAW_ATTRIB_NOINLINE void test_zip_is_sized( ) {
		auto a = std::vector<int>{ 1, 2, 3, 4, 5 };
		auto b = std::vector<int>{ 1, 2, 3 };
		ensure_sized( zip_view( a, a ), 5 );
		// The shortest range sets the size
		ensure_sized( zip_view( a, b ), 3 );
		ensure_sized( zip_view( b, a, a ), 3 );
		ensure_sized( Zip( a, b ), 3 );
	}

	DAW_ATTRIB_NOINLINE void test_zip_of_iota_is_sized( ) {
		ensure_sized(
		  zip_view( iota_view<int>( 0, 10 ), iota_view<int>( 5, 10 ) ), 5 );
	}

	DAW_ATTRIB_NOINLINE void test_enumerate_is_sized( ) {
		auto a = std::vector<int>{ 1, 2, 3, 4, 5 };
		ensure_sized( pipeline( a, Enumerate ), 5 );
		ensure_sized( pipeline( a, Skip( 1 ), Enumerate ), 4 );
	}

	DAW_ATTRIB_NOINLINE void test_take_of_zip_is_sized( ) {
		auto a = std::vector<int>{ 1, 2, 3, 4, 5 };
		ensure_sized( pipeline( zip_view( a, a ), Take( 2 ) ), 2 );
	}

	DAW_ATTRIB_NOINLINE void test_zip_distance_after_moving( ) {
		auto a = std::vector<int>{ 1, 2, 3, 4, 5 };
		auto b = std::vector<int>{ 1, 2, 3, 4 };
		auto z = zip_view( a, b );
		auto it = std::begin( z );
		++it;
		daw_ensure( std::end( z ) - it == 3 );
		daw_ensure( it - std::end( z ) == -3 );
		it += 2;
		daw_ensure( std::end( z ) - it == 1 );
		--it;
		daw_ensure( std::end( z ) - it == 2 );
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

	// std::list knows its size but cannot subtract iterators, so the size comes
	// from size( ).  The shortest range sets the size
	DAW_ATTRIB_NOINLINE void test_zip_size_over_sized_bidirectional_ranges( ) {
		for( std::size_t n = 0; n < 10; ++n ) {
			auto l = std::list<int>( );
			auto v = std::vector<int>( );
			for( std::size_t i = 0; i < n; ++i ) {
				l.push_back( static_cast<int>( i ) );
				v.push_back( static_cast<int>( i ) );
			}
			v.push_back( 42 );
			ensure_size_matches( zip_view( l, l ), n );
			ensure_size_matches( zip_view( v, l ), n );
			ensure_size_matches( pipeline( l, Enumerate ), n );
			// Enumerating from an offset zips with an unbounded iota_view
			ensure_size_matches( pipeline( l, EnumerateFrom( std::size_t{ 5 } ) ),
			                     n );
		}
	}

	DAW_ATTRIB_NOINLINE void test_zip_has_no_size_over_unsized_range( ) {
		auto fl = std::forward_list<int>{ 1, 2, 3 };
		auto v = std::vector<int>{ 1, 2, 3 };
		auto z = zip_view( v, fl );
		static_assert( not has_size_member<decltype( z )> );
	}

	// std::ranges::filter_view only has a non-const begin( ).  Zipping it as an
	// lvalue refers to it, and as a temporary the zip owns it
	DAW_ATTRIB_NOINLINE void test_zip_of_filter_view( ) {
		auto v = std::vector<int>{ 1, 2, 3, 4, 5, 6 };
		auto other = std::vector<int>{ 10, 20, 30, 40 };
		auto even = []( int x ) {
			return x % 2 == 0;
		};
		auto fv = v | std::views::filter( even );

		auto check = []( auto &&z ) {
			auto firsts = std::vector<int>{ };
			auto seconds = std::vector<int>{ };
			for( auto &&[a, b] : z ) {
				firsts.push_back( a );
				seconds.push_back( b );
			}
			daw_ensure( ( firsts == std::vector<int>{ 2, 4, 6 } ) );
			daw_ensure( ( seconds == std::vector<int>{ 10, 20, 30 } ) );
		};
		check( zip_view( fv, other ) );
		check( zip_view( v | std::views::filter( even ), other ) );
		check( Zip( fv, other ) );

		auto indices = std::vector<std::size_t>{ };
		for( auto &&[idx, value] : pipeline( fv, Enumerate ) ) {
			indices.push_back( idx );
			daw_ensure( value % 2 == 0 );
		}
		daw_ensure( ( indices == std::vector<std::size_t>{ 0, 1, 2 } ) );
	}
} // namespace tests

int main( ) {
	tests::test_zip_is_sized( );
	tests::test_zip_of_iota_is_sized( );
	tests::test_enumerate_is_sized( );
	tests::test_take_of_zip_is_sized( );
	tests::test_zip_distance_after_moving( );
	tests::test_zip_size_over_sized_bidirectional_ranges( );
	tests::test_zip_has_no_size_over_unsized_range( );
	tests::test_zip_of_filter_view( );
}
