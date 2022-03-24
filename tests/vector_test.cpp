// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_algorithm.h>
#include <daw/daw_consteval.h>
#include <daw/vector.h>

#include <cstddef>
#include <iostream>

DAW_CONSTEVAL int sum( std::size_t n ) {
	auto v = daw::vector<int>(
	  daw::do_resize_and_overwrite, n, []( int *ptr, std::size_t m ) {
		  for( std::size_t idx = 0; idx < m; ++idx ) {
			  std::construct_at( &ptr[idx], static_cast<int>( idx ) );
		  }
		  return m;
	  } );
	return daw::algorithm::accumulate( v.begin( ), v.end( ), 0 );
}

int main( ) {
	daw::vector<int> x;
	x.reserve( 3 );
	x = { 1, 2, 3 };
	x[0] = 2;
	std::cout << "cap: " << x.capacity( ) << '\n';
	x.resize_and_overwrite( x.capacity( ) * 2, []( int *p, std::size_t sz, auto&&... ) {
		for( std::size_t n = 0; n < sz; ++n ) {
			std::construct_at( &p[n], n );
		}
		return sz;
	} );
	for( auto const &i : x ) {
		std::cout << i << '\n';
	}
	constexpr int a = sum( 10 );
	static_assert( a == 45 );
}
