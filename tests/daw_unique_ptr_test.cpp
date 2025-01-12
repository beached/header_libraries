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

#include <daw/daw_unique_ptr.h>

#include <cassert>
#include <memory>

struct Base {
	int x = 55;
	Base( ) = default;
	Base( int v )
	  : x( v ) {}
	virtual ~Base( ) = default;
};

struct Child : Base {
	Child( ) = default;
	Child( int v )
	  : Base( v ) {}
};

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
	int x = -1;
	my_base = std::move( my_base ).and_then( [&]( auto p ) {
		x = p->x;
		return p;
	} );
	assert( x == my_base->x );
	auto my_child = my_base.and_then( []( auto &mb ) {
		return daw::make_unique<Child>( mb->x );
	} );
	assert( my_child->x == my_base->x );
	daw::unique_ptr<Child> my_child2 = nullptr;
	auto x2 = my_child2
	            .or_else( []( daw::unique_ptr<Child> const & ) {
		            return daw::make_unique<Base>( 55 );
	            } )
	            .and_then( []( daw::unique_ptr<Base> ) {
		            return daw::make_unique<Child>( 42 );
	            } )
	            .and_then( []( auto const &p ) {
		            return p->x;
	            } );
	assert( x2 == 42 );
}
