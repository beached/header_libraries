// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_mutable_capture.h"
#include "daw/daw_stack_function.h"
#include "daw/daw_utility.h"

#include <functional>
#include <iostream>

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

static daw::function<100, int( int )> func_001 = []( int x ) -> int {
	return x * x;
};

void stack_function_test_001( ) {
	auto result = func_001( 4 );
	daw::expecting( 16, result );
}

void stack_function_test_002( ) {
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
	daw::function<100, int( )> f{ };
	daw::function<100, std::string( )> f2( callable{ } );
	std::cout << "f2 empty state:" << ( f2.empty( ) ? "empty" : "not empty" )
	          << '\n';
	std::cout << f2( ) << '\n';
	f2 = daw::function<100, std::string( )>{ };
	try {
		std::cout << f2( ) << '\n';
	} catch( std::bad_function_call const & ) {
		std::cout << "Successfully caught bad_function_call on empty function\n";
	}
	std::cout << "f2 empty state:" << ( f2.empty( ) ? "empty" : "not empty" )
	          << '\n';
	std::function<std::string( )> sf{ };
	f2 = sf;
	std::cout << "f2 empty(std::function that is empty) state:"
	          << ( f2.empty( ) ? "empty" : "not empty" ) << '\n';

	daw::function<100, void( )> vf1 = []( ) {
		std::cout << "void( )\n";
	};
	vf1( );
	daw::function<150, void( )> const cvf1 = []( ) {
		std::cout << "void( )\n";
	};
	cvf1( );

	daw::function<2000, void( )> fcvf1 = [_cvf1 =
	                                        daw::mutable_capture( cvf1 )]( ) {
		( *_cvf1 )( );
	};
	fcvf1( );
}

int main( ) {
	stack_function_test_001( );
	stack_function_test_002( );
}
