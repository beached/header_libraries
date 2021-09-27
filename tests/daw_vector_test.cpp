// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include "daw/daw_vector.h"

#include <cstddef>
#include <iostream>

auto a = Vector( { 1, 2, 3 } );

Vector<int> foo( std::size_t n ) {
	auto v = Vector<int>( sized_for_overwrite, std::max( std::size_t{ 128 }, n ),
	                      [maxsz = n]( int *p, std::size_t ) {
		                      for( std::size_t m = 0; m < maxsz; ++m ) {
			                      p[m] = static_cast<int>( m );
		                      }
		                      return maxsz;
	                      } );
	return v;
}

int main( ) {
	for( int v : a ) {
		std::cout << v << '\n';
	}
	for( int v : foo( 4 ) ) {
		std::cout << v << '\n';
	}

	std::cout << "\nb\n";
	auto b = foo( 6 );
	std::cout << "b cap:" << b.capacity( ) << '\n';
	for( int v : b ) {
		std::cout << v << '\n';
	}
	std::cout << "b.insert( begin( ) + 1, 3 )\n";
	b.insert( b.begin( ) + 1, 3 );
	std::cout << "b cap:" << b.capacity( ) << '\n';

	for( int v : b ) {
		std::cout << v << '\n';
	}

	std::cout << "b.insert( begin( ) + 3, {9,8,7}\n";
	b.insert( b.begin( ) + 3, { 9, 8, 7 } );
	std::cout << "b cap:" << b.capacity( ) << '\n';

	for( int v : b ) {
		std::cout << v << '\n';
	}

	std::cout << "b size:" << b.size( ) << '\n';
	std::cout << "b cap:" << b.capacity( ) << '\n';
	for( int n = 0; n < 4096; ++n ) {
		b.insert( b.data( ) + ( b.size( ) / 2 ), { 1, 2, 3, 4, 5 } );
		std::cout << "b size:" << b.size( ) << '\n';
		std::cout << "b cap:" << b.capacity( ) << '\n';
	}
}