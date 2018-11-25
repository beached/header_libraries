// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <functional>
#include <iostream>
#include <string>

#include "daw/daw_stack_function.h"

std::string strfunc( ) {
	return "Goodbye";
}

struct callable {
	template<typename... Args>
	std::string operator( )( Args &&... ) const noexcept {
		return "callable";
	}
	callable( ) noexcept = default;
	callable( callable const & ) noexcept = default;
	callable( callable && ) noexcept = default;
	callable &operator=( callable const & ) noexcept = default;
	callable &operator=( callable && ) noexcept = default;
	~callable( ) {
		std::cout << "~operator( )\n";
	}
};

int main( ) {
	daw::function<100, std::string( )> func = []( ) -> std::string {
		return "Hello";
	};
	daw::function<100, std::string( )> const func2 = []( ) -> std::string {
		return "Hello2";
	};

	std::cout << func( ) << '\n' << func2( ) << '\n';
	func = strfunc;
	std::cout << func( ) << '\n';
	func = &strfunc;
	std::cout << func( ) << '\n';
	daw::function<100, int( )> f{};
	daw::function<100, std::string( )> f2( callable{} );
	std::cout << "f2 empty state:" << ( f2.empty( ) ? "empty" : "not empty" )
	          << '\n';
	std::cout << f2( ) << '\n';
	f2 = daw::function<100, std::string( )>{};
	try {
		std::cout << f2( ) << '\n';
	} catch( std::bad_function_call const & ) {
		std::cout << "Successfully caught bad_function_call on empty function\n";
	}
	std::cout << "f2 empty state:" << ( f2.empty( ) ? "empty" : "not empty" )
	          << '\n';
	std::function<std::string( )> sf{};
	f2 = sf;
	std::cout << "f2 empty(std::function that is empty) state:"
	          << ( f2.empty( ) ? "empty" : "not empty" ) << '\n';

	daw::function<100, void( )> vf1 = []( ) { std::cout << "void( )\n"; };
	vf1( );
	daw::function<150, void( )> const cvf1 = []( ) { std::cout << "void( )\n"; };
	cvf1( );

	daw::function<2000, void( )> fcvf1 =
	  [cvf1 = daw::mutable_capture( cvf1 )]( ) { ( *cvf1 )( ); };
	fcvf1( );
	return 0;
}
