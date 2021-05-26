// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//
//

#include <daw/daw_ring_adaptor.h>

#include <cassert>

int main( ) {
	using rb_t = daw::ring_adaptor<int[10]>;
	rb_t buff{ };

	assert( buff.is_empty( ) );

	for( std::size_t n = 0; n < buff.capacity( ); ++n ) {
		buff.push_back( static_cast<int>( n ) );
	}

	assert( buff.is_full( ) );

	for( std::size_t n = 0; n < buff.capacity( ); ++n ) {
		(void)buff.pop_front( );
	}

	assert( buff.is_empty( ) );

	for( std::size_t n = 0; n < buff.capacity( ); ++n ) {
		buff.push_back( static_cast<int>( n * 2 ) );
	}

	assert( buff.is_full( ) );


	int sum = 0;
	for( int i : buff ) {
		sum += i;
	}
	assert( sum == 90 );
}
