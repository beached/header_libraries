// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_tuple_helper.h"

#include <iostream>
#include <tuple>

using namespace daw::tuple;

void daw_tuple_operators001( ) {
	using namespace daw::tuple::operators;
	auto t1 = std::make_tuple( 1.0, 2.0, 3.0 );
	auto res1 = t1 * 2.0;
	auto res2 = 2.0 * t1;
	std::cout << "result of 2*" << t1 << " => " << res1 << std::endl;
	std::cout << "result of " << t1 << "*2 => " << res2 << std::endl;

	auto t2 = std::make_tuple( 4.0, 2.0, 1.0 );
	auto res3 = t1 / t2;
	auto res6 = 1.0 + t1;
	auto res7 = t1 + 1.0;
	auto res8 = 1.0 / t1;
	auto res9 = t1 / 2.0;
	auto res10 = 1.0 - t1;
	auto res11 = t1 - 1.0;
	auto res12 = 2.0 * t1;
	auto res13 = t1 * 2.0;
	auto res14 = t1 + t2;
	auto res15 = t1 - t2;
	auto res16 = t1 * t2;

	std::cout << "result of 1.0 + " << t1 << " => " << res6 << std::endl;
	std::cout << "result of " << t1 << " + 2.0 => " << res7 << std::endl;

	std::cout << "result of 1.0 - " << t1 << " => " << res10 << std::endl;
	std::cout << "result of " << t1 << " - 1.0 => " << res11 << std::endl;

	std::cout << "result of 2.0 * " << t1 << " => " << res12 << std::endl;
	std::cout << "result of " << t1 << " * 2.0 => " << res13 << std::endl;

	std::cout << "result of 1.0 / " << t1 << " => " << res8 << std::endl;
	std::cout << "result of " << t1 << " / 2.0 => " << res9 << std::endl;

	std::cout << "result of " << t1 << " + " << t2 << " => " << res14
	          << std::endl;
	std::cout << "result of " << t1 << " - " << t2 << " => " << res15
	          << std::endl;
	std::cout << "result of " << t1 << " * " << t2 << " => " << res16
	          << std::endl;
	std::cout << "result of " << t1 << " / " << t2 << " => " << res3 << std::endl;

	daw::expecting( res1 == res2 );
	daw::expecting( res3 == std::make_tuple( 0.25, 1.0, 3.0 ) );
	daw::expecting( res6 == std::make_tuple( 2.0, 3.0, 4.0 ) );
	daw::expecting( res7 == std::make_tuple( 2.0, 3.0, 4.0 ) );
	daw::expecting( res8 == std::make_tuple( 1.0, 0.5, 1.0 / 3.0 ) );
	daw::expecting( res9 == std::make_tuple( 0.5, 1.0, 1.5 ) );
	daw::expecting( res10 == std::make_tuple( 0.0, -1.0, -2.0 ) );
	daw::expecting( res11 == std::make_tuple( 0.0, 1.0, 2.0 ) );
	daw::expecting( res12 == std::make_tuple( 2.0, 4.0, 6.0 ) );
	daw::expecting( res13 == std::make_tuple( 2.0, 4.0, 6.0 ) );
	daw::expecting( res14 == std::make_tuple( 5.0, 4.0, 4.0 ) );
	daw::expecting( res15 == std::make_tuple( -3.0, 0.0, 2.0 ) );
	daw::expecting( res16 == std::make_tuple( 4.0, 4.0, 3.0 ) );
	daw::expecting( res3 == std::make_tuple( 0.25, 1.0, 3.0 ) );
}

void daw_tuple_helpers_001( ) {
	using namespace daw::tuple;
	auto t1 = std::make_tuple( 5, 4, 3, 1 );
	auto t2 = std::make_tuple( 1, 2, 4, 8 );

	auto t_min = min( t1, t2 );
	auto t_max = max( t1, t2 );
	std::cout << "min of " << t1 << " and " << t2 << " is " << t_min << std::endl;
	std::cout << "max of " << t1 << " and " << t2 << " is " << t_max << std::endl;
	daw::expecting( t_min == std::make_tuple( 1, 2, 3, 1 ) );
	daw::expecting( t_max == std::make_tuple( 5, 4, 4, 8 ) );
}

void runtime_get_test_001( ) {
	constexpr auto tp = std::make_tuple( "hello", 3ULL, 3.545, true );
	auto i = 0ULL;
	daw::tuple::apply_at( tp, 1, [&i]( unsigned long long value ) {
		i = static_cast<unsigned long long>( value );
	} );
	daw::expecting( 3ULL, i );
}

static_assert( daw::tuple_size_v<std::tuple<int, int, int>> == 3 );
static_assert( daw::tuple_size_v<std::tuple<>> == 0 );
static_assert( daw::tuple_size_v<std::tuple<int, int, int> const> == 3 );
static_assert( daw::tuple_size_v<std::tuple<> const> == 0 );
static_assert( daw::is_empty_tuple_v<std::tuple<>> );
static_assert( daw::is_empty_tuple_v<std::tuple<> const> );

int main( ) {
	daw_tuple_operators001( );
	daw_tuple_helpers_001( );
	runtime_get_test_001( );
}
