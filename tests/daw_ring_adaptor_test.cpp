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
	using rb_t = daw::ring_adaptor<int[64]>;
	rb_t buff{ };

	assert( buff.is_empty( ) );

	for( std::size_t n = 0; n < buff.capacity( ); ++n ) {
		buff.push_back( n );
	}

	assert( buff.is_full( ) );

	for( std::size_t n = 0; n < buff.capacity( ); ++n ) {
		(void)buff.pop_front( );
	}

	assert( buff.is_empty( ) );

	for( std::size_t n = 0; n < buff.capacity( ); ++n ) {
		buff.push_back( n*2 );
	}

	assert( buff.is_full( ) );
}
