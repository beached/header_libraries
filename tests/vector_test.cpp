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
	for( std::size_t x = 0; x < 1024; ++x ) {
		v.push_back( static_cast<int>( x ) );
	}
	for( std::size_t x = 0; x < 1024; ++x ) {
		v.pop_back( );
	}

	return daw::algorithm::accumulate( v.begin( ), v.end( ), 0 );
}

DAW_CONSTEVAL bool test( ) {
	auto y = daw::vector<int>( daw::do_resize_and_overwrite, 100,
	                           []( int *ptr, std::size_t sz ) {
		                           for( std::size_t n = 0; n < sz; ++n ) {
			                           std::construct_at( &ptr[n], n );
		                           }
		                           return sz;
	                           } );
	y.resize_and_overwrite( 50, []( int *ptr, std::size_t sz ) {
		for( std::size_t n = 0; n < sz; ++n ) {
			std::construct_at( &ptr[n], n );
		}
		return sz;
	} );
	return y.size( ) == 50;
}

int main( ) {
	daw::vector<int> x;
	x.reserve( 3 );
	x = { 1, 2, 3 };
	x[0] = 2;
	std::cout << "cap: " << x.capacity( ) << '\n';
	x.append_and_overwrite( x.capacity( ) * 2, []<typename Alloc>( int *p, std::size_t sz, Alloc & alloc ) {
		for( std::size_t n = 0; n < sz; ++n ) {
			std::construct_at( &p[n], n );
		}
		return sz;
	} );
	for( auto const &i : x ) {
		std::cout << i << '\n';
	}

	static_assert( sum( 10 ) == 45 );
	static_assert( test( ) );
}
