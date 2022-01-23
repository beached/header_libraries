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
	  a, []( int v ) { return v == 5.5; }, []( double d ) { return d == 5.5; } );
	daw::expecting( result );
	return true;
}
static_assert( visit_nt_001( ) );

constexpr bool visit_nt_002( ) {
	std::variant<int, double> a = 5.5;
	std::optional<bool> result;
	::daw::visit_nt( a, [&result]( auto v ) { result = ( v == 5.5 ); } );
	daw::expecting( result );
	daw::expecting( *result );
	return true;
}
static_assert( visit_nt_002( ) );

constexpr bool visit_nt_003( ) {
	std::variant<int, double> a = 5.5;
	auto result = ::daw::visit_nt<bool>(
	  a, []( int v ) { return v == 5.5; }, []( double d ) { return d == 5.5; } );
	daw::expecting( result );
	return true;
}
static_assert( visit_nt_003( ) );

constexpr bool func_004( int ) noexcept {
	return true;
}

constexpr bool visit_nt_004( ) {
	std::variant<int, double> a = 5;
	auto result = ::daw::visit_nt( a, func_004, []( double ) { return false; } );
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

int main( ) {
	visit_nt_004( );
	visit_nt_005( );
}
