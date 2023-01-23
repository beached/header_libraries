// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

// #define DAW_NO_SAFE_POINTER_CHECKS

#include <daw/daw_tracked_allocator.h>
#include <daw/vector.h>

#include <map>
#include <utility>
#include <vector>

int main( ) {
	{
		auto v =
		  std::vector<std::size_t, daw::memory::tracked_allocator<std::size_t>>{ };
		v.resize( 123 );
		for( size_t n = 0; n < 123; ++n ) {
			v[n] = n;
		}
		(void)v;
	}
	{
		auto v =
		  daw::vector<std::size_t, daw::memory::tracked_allocator<std::size_t>>{ };
		(void)v.append_and_overwrite( 123U, []( auto ptr, std::size_t sz ) {
			for( std::size_t n = 0; n < sz; ++n ) {
				ptr[n] = n;
			}
			return sz;
		} );
		(void)v;
	}
	{
		using string_t = std::basic_string<char, std::char_traits<char>,
		                                   daw::memory::tracked_allocator<char>>;
		using pair_t = std::pair<string_t const, int>;
		using Allocator =
		  daw::memory::tracked_allocator<pair_t, std::allocator<pair_t>>;
		auto m = std::map<string_t, int, std::less<string_t>, Allocator>( );
		(void)m;
	}
}