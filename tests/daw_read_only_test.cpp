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
	auto test_rwtype( daw::read_only<T> &lhs, daw::read_only<T> const &rhs )
	  -> decltype( lhs = rhs );

	template<typename T>
	using rw_type = decltype( test_rwtype( std::declval<daw::read_only<T> &>( ),
	                                       std::declval<daw::read_only<T>>( ) ) );

#if not defined( _MSC_VER )
	static_assert( not daw::is_detected_v<rw_type, int>,
	               "Read only type is assignable, it should not be" );
#endif
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

struct read_only_test_004_t {
	daw::read_only<int, read_only_test_004_t> x{ };

	constexpr void set_x( int v ) {
		x.value = v;
	}
};

constexpr std::true_type read_only_test_004( ) {
	auto v = read_only_test_004_t{ 55 };
	v.set_x( 52 );

	return { };
}
static_assert( read_only_test_004( ) );

int main( ) {
	daw::expecting( not daw::is_detected_v<test_01::rw_type, int> );	
	test_02::daw_read_only_test_02( );
}
