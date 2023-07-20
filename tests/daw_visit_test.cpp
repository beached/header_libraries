// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_visit.h"

#include <optional>
#include <variant>

constexpr bool visit_nt_001( ) {
	std::variant<int, double> a = 5.5;
	auto result = ::daw::visit_nt(
	  a,
	  []( int v ) {
		  return v == 5.5;
	  },
	  []( double d ) {
		  return d == 5.5;
	  } );
	daw::expecting( result );
	return true;
}
static_assert( visit_nt_001( ) );

constexpr bool visit_nt_002( ) {
	std::variant<int, double> a = 5.5;
	std::optional<bool> result;
	::daw::visit_nt( a, [&result]( auto v ) {
		result = ( v == 5.5 );
	} );
	daw::expecting( result );
	daw::expecting( *result );
	return true;
}
static_assert( visit_nt_002( ) );

constexpr bool visit_nt_003( ) {
	std::variant<int, double> a = 5.5;
	auto result = ::daw::visit_nt<bool>(
	  a,
	  []( int v ) {
		  return v == 5.5;
	  },
	  []( double d ) {
		  return d == 5.5;
	  } );
	daw::expecting( result );
	return true;
}
static_assert( visit_nt_003( ) );

constexpr bool func_004( int ) noexcept {
	return true;
}

constexpr bool visit_nt_004( ) {
	std::variant<int, double> a = 5;
	auto result = ::daw::visit_nt( a, func_004, []( double ) {
		return false;
	} );
	daw::expecting( result );
	return true;
}
#ifndef WIN32
static_assert( visit_nt_004( ) );
#endif

template<typename T>
constexpr void func_005( T ) {}

constexpr bool visit_nt_005( ) {
	std::variant<int, double> a = 5;
	::daw::visit_nt( a, func_005<int>, func_005<double> );
	return true;
}
#ifndef WIN32
static_assert( visit_nt_005( ) );
#endif

struct A {
	constexpr bool operator( )( int ) const {
		return true;
	}
};
static_assert( ::daw::is_visitable_v<int, A> );

struct B {};
static_assert( !::daw::is_visitable_v<int, B> );

namespace test_006 {
	struct T0 {};
	struct T1 {};
	struct T2 {};
	struct T3 {};
	struct T4 {};
	struct T5 {};
	struct T6 {};
	struct T7 {};
	struct T8 {};
	struct T9 {};
	struct T10 {};
	struct T11 {};
	struct T12 {};
	struct T13 {};
	struct T14 {};
	struct T15 {};
	struct T16 {};
	constexpr std::variant<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12,
	                       T13, T14, T15, T16>
	foobar( int n ) {
		switch( n ) {
		case 0:
			return T0{ };
		case 1:
			return T1{ };
		case 2:
			return T2{ };
		case 3:
			return T3{ };
		case 4:
			return T4{ };
		case 5:
			return T5{ };
		case 6:
			return T6{ };
		case 7:
			return T7{ };
		case 8:
			return T8{ };
		case 9:
			return T9{ };
		case 10:
			return T10{ };
		case 11:
			return T11{ };
		case 12:
			return T12{ };
		case 13:
			return T13{ };
		case 14:
			return T14{ };
		case 15:
			return T15{ };
		case 16:
			return T16{ };
		}
		DAW_UNREACHABLE( );
	}
	static_assert( foobar( 0 ).index( ) == 0 );
	static_assert( foobar( 1 ).index( ) == 1 );
	static_assert( foobar( 2 ).index( ) == 2 );
	static_assert( foobar( 3 ).index( ) == 3 );
	static_assert( foobar( 4 ).index( ) == 4 );
	static_assert( foobar( 5 ).index( ) == 5 );
	static_assert( foobar( 6 ).index( ) == 6 );
	static_assert( foobar( 7 ).index( ) == 7 );
	static_assert( foobar( 8 ).index( ) == 8 );
	static_assert( foobar( 9 ).index( ) == 9 );
	static_assert( foobar( 10 ).index( ) == 10 );
	static_assert( foobar( 11 ).index( ) == 11 );
	static_assert( foobar( 12 ).index( ) == 12 );
	static_assert( foobar( 13 ).index( ) == 13 );
	static_assert( foobar( 14 ).index( ) == 14 );
	static_assert( foobar( 15 ).index( ) == 15 );
	static_assert( foobar( 16 ).index( ) == 16 );
} // namespace test_006

int main( ) {
	visit_nt_004( );
	visit_nt_005( );
}
