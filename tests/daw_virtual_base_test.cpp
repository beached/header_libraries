// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/daw_virtual_base.h"
#include "daw/daw_benchmark.h"

struct B: daw::virtual_base<B> {
	B( ) = default;
	virtual int func( ) {
		return 0;
	}
	~B( ) override;
};
B::~B( ) { }

struct C: B {
	C( ) = default;
	int func( ) override { 
		return 1;
	}
	~C( ) override;
};
C::~C( ) { }

int main( ) {
	std::unique_ptr<B> b = std::unique_ptr<B>( new C( ) );

	daw::expecting( b->func( ), 1 );
}