// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_exception.h"

#include <exception>
#include <memory>

void test_01( ) {
	daw::expecting( []( ) {
		std::unique_ptr<int> a;
		auto b = std::make_unique<int>( 3 );
		daw::exception::daw_throw_on_true( a );
		daw::exception::daw_throw_on_false( b );
		return true;
	}( ) );

	daw::expecting_exception(
	  []( ) { daw::exception::precondition_check<std::exception>( false ); } );
}

int main( ) {
	test_01( );
}
