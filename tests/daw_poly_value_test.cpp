// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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
	char get( ) const override {
		return 'C';
	}
};

struct Child2 : Base {
	bool *ptr = nullptr;
	Child2( bool *p )
	  : ptr( p ) {}
	Child2( Child2 const & ) = default;
	Child2( Child2 && ) = default;
	Child2 &operator=( Child2 const & ) = default;
	Child2 &operator=( Child2 && ) = default;
	~Child2( ) {
		*ptr = true;
	}
};

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
}
