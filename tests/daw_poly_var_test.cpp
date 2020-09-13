// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_poly_var.h"

#include <iostream>

namespace {
	struct Base {
		Base( ) = default;
		virtual ~Base( ) = default;
		Base( Base const & ) = default;
		Base( Base && ) = default;
		Base &operator=( Base const & ) = default;
		[[maybe_unused]] Base &operator=( Base && ) = default;

		virtual int get_num( ) = 0;
	};

	struct C1 : Base {
		C1( ) = default;
		int get_num( ) override {
			return 1;
		}
	};

	struct C2 : Base {
		C2( ) = default;
		int get_num( ) override {
			return 2;
		}
	};
} // namespace

int main( int argc, char ** ) {
	if( argc % 2 == 0 ) {
		daw::poly_var<Base, C1, C2> val( C2{ } );
		std::cout << "result: " << val->get_num( ) << '\n';
		return 0;
	}
	daw::poly_var<Base, C1, C2> val( C1{ } );
	std::cout << "result: " << val->get_num( ) << '\n';
	return 0;
}
