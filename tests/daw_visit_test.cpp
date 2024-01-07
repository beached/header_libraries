// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_visit.h"

#include <cassert>
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
struct T16 {
};
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
	}
	return T16{ };
}

int foofoo( std::variant<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12,
                         T13, T14, T15, T16>
              v ) {
	return daw::visit_nt(
	  v,
	  []( T0 ) {
		  return 0;
	  },
	  []( T1 ) {
		  return 1;
	  },
	  []( T2 ) {
		  return 2;
	  },
	  []( T3 ) {
		  return 3;
	  },
	  []( T4 ) {
		  return 4;
	  },
	  []( T5 ) {
		  return 5;
	  },
	  []( T6 ) {
		  return 6;
	  },
	  []( T7 ) {
		  return 7;
	  },
	  []( T8 ) {
		  return 8;
	  },
	  []( T9 ) {
		  return 9;
	  },
	  []( T10 ) {
		  return 10;
	  },
	  []( T11 ) {
		  return 11;
	  },
	  []( T12 ) {
		  return 12;
	  },
	  []( T13 ) {
		  return 13;
	  },
	  []( T14 ) {
		  return 14;
	  },
	  []( T15 ) {
		  return 15;
	  },
	  []( T16 ) {
		  return 16;
	  } );
}

int main( ) {
	visit_nt_004( );
	visit_nt_005( );
	assert( foofoo( foobar( 0 ) ) == 0 );
	assert( foofoo( foobar( 1 ) ) == 1 );
	assert( foofoo( foobar( 2 ) ) == 2 );
	assert( foofoo( foobar( 3 ) ) == 3 );
	assert( foofoo( foobar( 4 ) ) == 4 );
	assert( foofoo( foobar( 5 ) ) == 5 );
	assert( foofoo( foobar( 6 ) ) == 6 );
	assert( foofoo( foobar( 7 ) ) == 7 );
	assert( foofoo( foobar( 8 ) ) == 8 );
	assert( foofoo( foobar( 9 ) ) == 9 );
	assert( foofoo( foobar( 10 ) ) == 10 );
	assert( foofoo( foobar( 11 ) ) == 11 );
	assert( foofoo( foobar( 12 ) ) == 12 );
	assert( foofoo( foobar( 13 ) ) == 13 );
	assert( foofoo( foobar( 14 ) ) == 14 );
	assert( foofoo( foobar( 15 ) ) == 15 );
	assert( foofoo( foobar( 16 ) ) == 16 );
}
