// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_math.h"

#include "daw/daw_benchmark.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>

static_assert( daw::math::abs( std::numeric_limits<uintmax_t>::max( ) ) ==
               std::numeric_limits<uintmax_t>::max( ) );

static_assert(
  daw::math::abs( std::numeric_limits<intmax_t>::max( ) ) ==
  static_cast<uintmax_t>( std::numeric_limits<intmax_t>::max( ) ) );

static_assert( daw::math::abs( std::numeric_limits<uint8_t>::min( ) ) == 0 );

static_assert( daw::math::abs( std::numeric_limits<intmax_t>::min( ) ) ==
               static_cast<uintmax_t>( std::numeric_limits<intmax_t>::max( ) ) +
                 1ULL );

void show( std::string const &value ) {
	const auto width = std::numeric_limits<double>::digits10;
	std::cout << std::setiosflags( std::ios::fixed ) << std::setw( width + 2 )
	          << std::left << value << " ";
}

void show( double value ) {
	const auto width = std::numeric_limits<double>::digits10;
	std::cout << " ";
	std::cout << std::setiosflags( std::ios::fixed ) << std::setprecision( width )
	          << std::right << value;
}

void daw_math_factorial( ) {
	std::cout << "factorial( 5 ) = " << daw::math::factorial( 5 ) << "\n";
	daw::expecting_message( daw::math::factorial( 5 ) == 120,
	                        "1. factorial of 5 should be 120" );
	std::cout << "factorial( 0 ) = " << daw::math::factorial( 0 ) << "\n";
	daw::expecting_message( daw::math::factorial( 0 ) == 1,
	                        "2. factorial of 0 should return 1" );
}

constexpr bool daw_math_nearly_equal_test( ) {
	float f1 = 0;
	float f2 = 0;
	daw::expecting( daw::math::nearly_equal( f1, f2 ) );
	f1 = 1.0;
	daw::expecting( !daw::math::nearly_equal( f1, f2 ) );

	int i1 = 1;
	int i2 = 1;
	daw::expecting( daw::math::nearly_equal( i1, i2 ) );
	i2 = 2;
	daw::expecting( !daw::math::nearly_equal( i1, i2 ) );
	return true;
}

static_assert( daw::math::round( 4.6 ) == 5 );
static_assert( daw::math::nearly_equal( daw::math::round_by( 4.6, 7 ), 7.0 ) );
static_assert( daw::math::nearly_equal( daw::math::round_by( 4.6, 10 ), 0.0 ) );
static_assert( daw::math::nearly_equal( daw::math::ceil_by( 4.6, 1 ), 5.0 ) );
static_assert( daw::math::nearly_equal( daw::math::ceil_by( 4.6, 10 ), 10.0 ) );
static_assert( daw::math::nearly_equal( daw::math::ceil_by( 6.6, 10 ), 10.0 ) );
static_assert( daw::math::nearly_equal( daw::math::floor_by( 4.6, 1 ), 4.0 ) );
static_assert( daw::math::nearly_equal( daw::math::floor_by( 14.6, 10 ),
                                        10.0 ) );
static_assert( daw::math::nearly_equal( daw::math::floor_by( 6.6, 10 ), 0.0 ) );

static_assert( daw_math_nearly_equal_test( ) );

static_assert( daw::max( 5, 3, 100, 3, 4, 0xFFFF, -100 ) == 0xFFFF );

constexpr bool daw_max_inf_test( ) {
	daw::expecting(
	  daw::math::nearly_equal( daw::max( 5.0f,
	                                     3.0f,
	                                     std::numeric_limits<float>::infinity( ),
	                                     100.0f,
	                                     3.0f,
	                                     4.0f,
	                                     -100.0f ),
	                           std::numeric_limits<float>::infinity( ) ) );
	return true;
}
// Cannot test in constexpr until clang fixes bug
// static_assert( daw_max_inf_test( ) );

static_assert( daw::min( 5, 3, 100, 3, 4, -100, 0xFFFF ) == -100 );

static_assert(
  daw::math::nearly_equal( daw::min( 5.0f,
                                     3.0f,
                                     100.0f,
                                     std::numeric_limits<float>::infinity( ),
                                     3.0f,
                                     4.0f,
                                     -100.0f ),
                           -100.0f ) );
int main( ) {
	daw_math_factorial( );
	daw_max_inf_test( );
}
