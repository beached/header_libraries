// The MIT License (MIT)
//
// Copyright (c) 2019-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
	~Child2( );
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
		auto v2 = daw::poly_value<Base>( ptr );
	}
	daw::expecting( destructed );

	daw::expecting( 'C', func( v0 ) );

	auto v3 = daw::poly_value<Child>( );
	daw::expecting( 'C', func( v0 ) );

	daw::expecting( 'B', func( new Base( ) ) );
	daw::expecting( 'C', func( new Child( ) ) );

	Base b{};
	Child c{};
	daw::expecting( 'B', func( b ) );
	daw::expecting( 'C', func( c ) );
	auto v4 = daw::poly_value<Base>( );
	daw::expecting( 'B', func2( v4 ) );
	daw::expecting( 'C', func3( v3 ) );
	daw::expecting( 'C', func4( v3 ) );
	daw::expecting( 'C', func5( v3 ) );
	daw::expecting( 'C', func6( v3 ) );
}
