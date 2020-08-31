// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "daw/json/impl/daw_sbo.h"

#include <cstdio>

struct base {
	virtual ~base( );
	base( ) noexcept = default;
	base( base const & ) noexcept = default;
	base( base && ) noexcept = default;
	base &operator=( base const & ) noexcept = default;
	base &operator=( base && ) noexcept = default;

	virtual void func( ) {
		puts( "base" );
	}
};
base::~base( ) = default;

struct child : base {
	char data[12] = "Hello World";

	~child( ) override;
	child( ) noexcept = default;
	child( child const & ) noexcept = default;
	child( child && ) noexcept = default;
	child &operator=( child const & ) noexcept = default;
	child &operator=( child && ) noexcept = default;

	void func( ) override {
		puts( "child" );
	}
};
child::~child( ) = default;

int main( ) {
	daw::sbo_storage<sizeof( base )> storage{ };
	storage.allocate( child{ } );
	storage.template get<child>( )->func( );
	storage.deallocate<child>( );

	storage.allocate( base{ } );
	storage.template get<base>( )->func( );
	storage.deallocate<base>( );
}
