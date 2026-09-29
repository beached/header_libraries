// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/concat.h"

#include <daw/daw_attributes.h>
#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>

#include <cstddef>
#include <forward_list>
#include <iterator>
#include <list>
#include <ranges>
#include <tuple>
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
	Container make_range( std::size_t n ) {
		auto c = Container( );
		for( std::size_t i = 0; i < n; ++i ) {
			c.push_back( static_cast<int>( i ) );
		}
		return c;
	}

	// The size is the sum of the sizes, including when some of the ranges are
	// empty.  std::list knows its size but cannot subtract iterators
	DAW_ATTRIB_NOINLINE void test_concat_size( ) {
		for( std::size_t n = 0; n < 5; ++n ) {
			for( std::size_t m = 0; m < 5; ++m ) {
				auto v = make_range<std::vector<int>>( n );
				auto l = make_range<std::list<int>>( m );
				auto v2 = make_range<std::vector<int>>( m );
				ensure_size_matches( Concat( v, l ), n + m );
				ensure_size_matches( Concat( l, v ), n + m );
				ensure_size_matches( Concat( v, v2 ), n + m );
				ensure_size_matches( Concat( v, l, v2 ), n + m + m );
			}
		}
	}

	DAW_ATTRIB_NOINLINE void test_concat_size_of_tuple_of_ranges( ) {
		auto a = std::vector<int>{ 1, 2, 3 };
		auto b = std::list<int>{ 4, 5 };
		ensure_size_matches( Concat( std::tie( a, b ) ), 5 );
	}

	DAW_ATTRIB_NOINLINE void test_concat_size_of_zip( ) {
		auto a = std::vector<int>{ 1, 2, 3 };
		auto b = std::vector<int>{ 4, 5, 6 };
		ensure_size_matches( pipeline( zip_view( a, b ), Concat ), 6 );
	}

	// Every range must know its size without walking for Concat to have a
	// size( )
	DAW_ATTRIB_NOINLINE void test_concat_has_no_size_over_unsized_range( ) {
		auto v = std::vector<int>{ 1, 2, 3 };
		auto fl = std::forward_list<int>{ 4, 5 };
		auto r = Concat( v, fl );
		static_assert( not has_size_member<decltype( r )> );
	}
} // namespace tests

int main( ) {
	tests::test_concat_size( );
	tests::test_concat_size_of_tuple_of_ranges( );
	tests::test_concat_size_of_zip( );
	tests::test_concat_has_no_size_over_unsized_range( );
}
