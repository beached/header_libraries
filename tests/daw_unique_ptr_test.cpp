// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <daw/daw_unique_ptr.h>
#include <memory>

struct Base {
	virtual ~Base( ) = default;
};

struct Child : Base {};

int main( ) {
	auto a1 = daw::make_unique<int>( 5 );
	assert( a1 );
	assert( *a1 == 5 );
	auto a2 = daw::make_unique<int>( 5 );
	assert( a2 );
	assert( *a2 == 5 );
	assert( a1 != a2 );
	auto b = daw::make_unique<int[]>( 5 );
	assert( b );
	b[0] = 0;
	b[1] = 1;
	b[2] = 2;
	b[3] = 3;
	b[4] = 4;
	assert( b[1] == 1 );
	auto c = daw::make_unique_array<int[]>( 1, 2, 3, 4, 5 );
	assert( c );
	assert( c[1] == 2 );
	c.reset( );
	assert( not c );
	c = std::move( b );
	assert( c );
	assert( not b );
	assert( c[1] == 1 );
	auto d = std::move( a1 );
	assert( d );
	assert( not a1 );
	assert( *d == 5 );
	a2.reset( );
	assert( not a2 );

	std::default_delete<int>{ };
	daw::unique_ptr<Base> my_base = daw::make_unique<Child>( );
}
