// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_can_constant_evaluate.h>

#include <daw/daw_cpp_feature_check.h>

#include <string>

struct NotCXExpr {
	void operator( )( ) const {}
};
inline static constexpr auto NotCXExpr_v = NotCXExpr{ };
static_assert( not daw::can_constant_evaluate_v<NotCXExpr_v> );

struct CXExpr {
	constexpr void operator( )( ) const {}
};
inline static constexpr auto CXExpr_v = CXExpr{ };

static_assert( daw::can_constant_evaluate_v<CXExpr_v> );

#if false // TODO FIX or get rid of
static_assert(
  not daw::can_potentially_constant_construct_v<std::string, std::string> );
#endif

struct CXAggEmpty {};
static_assert( daw::can_potentially_constant_construct_v<CXAggEmpty> );
struct CXAggInt {
	int a;
};
static_assert( daw::can_potentially_constant_construct_v<CXAggInt, int> );

static_assert( daw::can_potentially_constant_construct_v<int, int> );

class Foo {
public:
	Foo( int ) {}
};
static_assert( not daw::can_potentially_constant_construct_v<Foo, int> );
class CFoo {
public:
	constexpr CFoo( int ) {}
};
static_assert( daw::can_potentially_constant_construct_v<CFoo, int> );

class Bar {
public:
	Bar( Foo ) {}
};
static_assert( not daw::can_potentially_constant_construct_v<Bar, Foo> );

class CBar {
public:
	CBar( CFoo ) {}
};
static_assert( not daw::can_potentially_constant_construct_v<CBar, CFoo> );

int main( ) {}