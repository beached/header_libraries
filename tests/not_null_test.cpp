// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_exception.h"
#include "daw/not_null.h"

#include <memory>

void daw_not_null_test_001( ) {
	auto tst = std::make_unique<int>( 5 );
	daw::not_null<int *> t = tst.get( );
	std::unique_ptr<int> tst2;

	daw::expecting_exception<daw::exception::NullPtrAccessException>(
	  [&]( ) { t = tst2.get( ); } );
}

int main( ) {
	daw_not_null_test_001( );
}
