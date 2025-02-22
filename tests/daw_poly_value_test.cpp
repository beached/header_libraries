// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_poly_value.h"

struct Base {
	Base( ) = default;
	Base( Base const & ) = default;
	Base( Base && ) noexcept = default;
	Base &operator=( Base const & ) = default;
	Base &operator=( Base && ) noexcept = default;
	virtual ~Base( );

	virtual char get( ) const {
		return 'B';
	}
};
Base::~Base( ) = default;

struct Child : Base {
	Child( ) = default;
	Child( Child const & ) = default;
	Child( Child && ) = default;
	Child &operator=( Child const & ) = default;
	Child &operator=( Child && ) = default;
	~Child( ) override;
	char get( ) const override {
		return 'C';
	}
};
Child::~Child( ) {}

struct Child2 : Base {
	bool *ptr = nullptr;
	Child2( bool *p )
	  : ptr( p ) {}
	Child2( Child2 const & ) = default;
	Child2( Child2 && ) = default;
	Child2 &operator=( Child2 const & ) = default;
	Child2 &operator=( Child2 && ) = default;
	~Child2( ) override;
};
Child2::~Child2( ) {
	*ptr = true;
}

char func( daw::poly_value<Base> const &p ) {
	return p->get( );
}

char func2( Base b ) {
	return b.get( );
}

char func3( Base &b ) {
	return b.get( );
}

char func4( Base const &b ) {
	return b.get( );
}

char func5( Base *b ) {
	return b->get( );
}

char func6( Base const *b ) {
	return b->get( );
}

int main( ) {
	auto v0 = daw::poly_value<Base>( );
	auto v1 = daw::poly_value<Base>( daw::construct_emplace<Child> );
	daw::expecting( 'B', v0->get( ) );
	daw::expecting( 'C', v1->get( ) );
	v0 = v1;
	daw::expecting( 'C', v0->get( ) );
	bool destructed = false;
	{
		auto ptr = new Child2( &destructed );
		auto v2 = daw::poly_value<Base>( std::move( *ptr ) );
	}
	daw::expecting( destructed );

	daw::expecting( 'C', func( v0 ) );

	auto v3 = daw::poly_value<Child>( );
	daw::expecting( 'C', func( v0 ) );

	daw::expecting( 'B', func( Base( ) ) );
	daw::expecting( 'C', func( Child( ) ) );

	Base b{ };
	Child c{ };
	daw::expecting( 'B', func( b ) );
	daw::expecting( 'C', func( c ) );
	auto v4 = daw::poly_value<Base>( );
	daw::expecting( 'B', func2( v4 ) );
	daw::expecting( 'C', func3( v3 ) );
	daw::expecting( 'C', func4( v3 ) );
	daw::expecting( 'C', func5( v3.get( ) ) );
	daw::expecting( 'C', func6( v3.get( ) ) );
}
