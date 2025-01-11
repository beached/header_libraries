// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_observer_ptr.h>

struct Base {
	int x = 33;
};
struct Child : Base {};

int main( int, char **argv ) {
	auto p = daw::observer_ptr( argv[0] );
	daw_ensure( p == argv[0] );
	auto ch = Child{ };
	auto c = daw::observer_ptr( &ch );
	Base *b = c;
	(void)b;

	auto x = c.and_then( []( daw::observer_ptr<Child> o ) {
		          o->x = 42;
		          return o;
	          } )
	           .or_else( []( daw::observer_ptr<Child> o ) {
		           return o;
	           } );
	daw_ensure( x->x == 42 );
}
