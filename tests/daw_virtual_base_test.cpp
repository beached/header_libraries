// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_virtual_base.h"

#include <memory>

struct B : daw::virtual_base<B> {
	B( ) = default;
	virtual int func( ) {
		return 0;
	}
	~B( ) override;
};
B::~B( ) {}

struct C : B {
	C( ) = default;
	int func( ) override {
		return 1;
	}
	~C( ) override;
};
C::~C( ) {}

int main( ) {
	std::unique_ptr<B> b = std::unique_ptr<B>( new C( ) );

	daw::expecting( b->func( ), 1 );
}
