// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/cache_last_view.h"

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
	void check_cache_last_size( ) {
		for( int n = 0; n < 10; ++n ) {
			auto c = Container( );
			for( int i = 0; i < n; ++i ) {
				c.push_back( i );
			}
			ensure_size_matches( pipeline( c, CacheLast ),
			                     static_cast<std::size_t>( n ) );
		}
	}

	DAW_ATTRIB_NOINLINE void test_cache_last_size_over_random_range( ) {
		check_cache_last_size<std::vector<int>>( );
	}

	// std::list knows its size but cannot subtract iterators
	DAW_ATTRIB_NOINLINE void
	test_cache_last_size_over_sized_bidirectional_range( ) {
		check_cache_last_size<std::list<int>>( );
	}

	// Only a range that knows its size without walking gives CacheLast a size( )
	DAW_ATTRIB_NOINLINE void test_cache_last_has_no_size_over_unsized_range( ) {
		auto fl = std::forward_list<int>{ 1, 2, 3 };
		auto r = pipeline( fl, CacheLast );
		static_assert( not has_size_member<decltype( r )> );
	}
} // namespace tests

int main( ) {
	tests::test_cache_last_size_over_random_range( );
	tests::test_cache_last_size_over_sized_bidirectional_range( );
	tests::test_cache_last_has_no_size_over_unsized_range( );
}
