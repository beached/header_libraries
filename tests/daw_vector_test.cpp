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

namespace daw {
	auto a = Vector( { 1, 2, 3 } );

	Vector<int> foo( std::size_t n ) {
		auto v =
		  Vector<int>( sized_for_overwrite, std::max( std::size_t{ 128 }, n ),
		               [maxsz = n]( int *p, std::size_t ) {
			               for( std::size_t m = 0; m < maxsz; ++m ) {
				               p[m] = static_cast<int>( m );
			               }
			               return maxsz;
		               } );
		return v;
	}
} // namespace daw

int main( ) {
	for( int v : daw::a ) {
		std::cout << v << '\n';
	}
	auto a2 = daw::a;
	for( int v : daw::foo( 4 ) ) {
		std::cout << v << '\n';
	}

	std::cout << "\nb\n";
	auto b = daw::foo( 6 );
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
	for( int n = 0; n < 4; ++n ) {
		b.insert( b.data( ) + ( b.size( ) / 2 ), { 1, 2, 3, 4, 5 } );
		std::cout << "b size:" << b.size( ) << '\n';
		std::cout << "b cap:" << b.capacity( ) << '\n';
	}
	b.clear( );
	for( int n = 0; n < 300; ++n ) {
		std::cout << "b size:" << b.size( ) << '\n';
		std::cout << "b cap:" << b.capacity( ) << '\n';
		b.push_back( n );
		std::cout << "value: " << b.back( ) << '\n';
	}
	std::cout << "contents\n";
	std::cout << "b size:" << b.size( ) << '\n';
	std::cout << "b cap:" << b.capacity( ) << '\n';
	for( auto v : b ) {
		std::cout << v << '\n';
	}
	auto v0 = daw::Vector<int>( );
	auto v1 = daw::Vector<int>( );
	v0 = v1;
	v1 = DAW_MOVE( v0 );
	(void)v0;
	(void)v1;
}