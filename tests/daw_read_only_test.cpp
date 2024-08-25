// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_read_only.h"

#include "daw/cpp_17.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_utility.h"

#include <cstddef>
#include <iostream>

namespace test_01 {
	template<typename T>
	using rw_type = decltype( daw::read_only<T>{ 4 } = 5 );

	static_assert( not daw::is_detected_v<rw_type, int>,
	               "Read only type is assignable, it should not be" );
} // namespace test_01

namespace test_02 {
	struct A {
		void b( ) {}
		void c( ) const {}
	};

	void daw_read_only_test_02( ) {
		std::cout << "sizeof( size_t ) -> " << sizeof( size_t );
		std::cout << " sizeof( int ) -> " << sizeof( int );
		std::cout << " sizeof( daw::read_only<int> ) -> "
		          << sizeof( daw::read_only<int> );
		std::cout << " sizeof( daw::read_only<size_t> ) -> "
		          << sizeof( daw::read_only<size_t> ) << '\n';
		constexpr daw::read_only<int> a{ };
		constexpr daw::read_only<int> b{ 1 };
		constexpr daw::read_only<int> c{ 2 };
		constexpr daw::read_only<int> d{ 1 };
		constexpr auto e = daw::read_only<int>( 5 );
		::Unused( e );

		constexpr daw::read_only<A> x{ };
		x->c( );

		// a & b
		daw::expecting( not( a == b ) );
		daw::expecting( not( b == a ) );
		daw::expecting( a != b );
		daw::expecting( b != a );
		daw::expecting( a < b );
		daw::expecting( not( b < a ) );
		daw::expecting( a <= b );
		daw::expecting( not( b <= a ) );
		daw::expecting( not( a >= b ) );
		daw::expecting( b >= a );
		daw::expecting( b == d );
		daw::expecting( b != c );
		daw::expecting( b < c );
		daw::expecting( b <= c );
		daw::expecting( c > b );
		daw::expecting( c >= b );
	}
} // namespace test_02

constexpr bool daw_read_only_test_03( ) {
	constexpr daw::read_only<int> a{ 5 };
	constexpr int b = a;
	daw::expecting( a, b );
	return true;
}
static_assert( daw_read_only_test_03( ) );

int main( ) {
	test_02::daw_read_only_test_02( );
}
