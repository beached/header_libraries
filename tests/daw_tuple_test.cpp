// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_tuple.h>

#include <cassert>

constexpr auto t0 = daw::tuple{ 1, 2, 3, 4 };
constexpr auto t1 = t0;
static_assert( t0 == t1 );
constexpr auto t2 = daw::tuple{ 1, 2, 3, 5 };
static_assert( t0 != t2 );

constexpr auto t3 = daw::tuple<>{ };
constexpr auto t4 = t3;
static_assert( t4 == t3 );
static_assert( t4 != t2 );

static_assert( daw::get<1>( t0 ) == 2 );
static_assert(
  std::is_same_v<double,
                 std::tuple_element_t<1, daw::tuple<int, double, int>>> );
static_assert(
  std::is_same_v<
    int,
    typename std::tuple_element<2, daw::tuple<int, int, int>>::type> );

static_assert( sizeof( daw::tuple<int, int, int> ) == sizeof( int ) * 3 );
struct empty {};
static_assert( sizeof( daw::tuple<int, int, int, empty> ) ==
               sizeof( int ) * 3 );

static_assert( std::is_trivially_copyable_v<daw::tuple<int, int, double>> );
static_assert( std::is_trivially_destructible_v<daw::tuple<int, int>> );

auto make_tuple( int &y ) {
	int x = 5;
	auto t = daw::tuple<int &, int, double, long>{ y, 5, 1.23, 6 };
	daw::get<0>( t ) = x;
	return t;
}

int main( ) {
	int x = 6;
	auto t = make_tuple( x );
	auto val = daw::get<0>( t );
	(void)val;
	assert( x == 5 );

	auto const add = []( int a, int b ) {
		return a + b;
	};

	static_assert( daw::apply( add, daw::make_tuple( 5, 6 ) ) == 11 );
}
