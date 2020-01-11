// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/daw_benchmark.h"
#include "daw/daw_math.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>

static_assert( daw::math::abs( std::numeric_limits<uintmax_t>::max( ) ) ==
                 std::numeric_limits<uintmax_t>::max( ),
               "" );

static_assert(
  daw::math::abs( std::numeric_limits<intmax_t>::max( ) ) ==
    static_cast<uintmax_t>( std::numeric_limits<intmax_t>::max( ) ),
  "" );

static_assert( daw::math::abs( std::numeric_limits<uint8_t>::min( ) ) == 0,
               "" );

static_assert(
  daw::math::abs( std::numeric_limits<intmax_t>::min( ) ) ==
    static_cast<uintmax_t>( std::numeric_limits<intmax_t>::max( ) ) + 1ULL,
  "" );

void show( std::string value ) {
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
	daw::expecting( daw::math::nearly_equal(
	  daw::max( 5, 3, std::numeric_limits<float>::infinity( ), 100, 3, 4, 0xFFFF,
	            -100 ),
	  std::numeric_limits<float>::infinity( ) ) );
	return true;
}
// Cannot test in constexpr until clang fixes bug
// static_assert( daw_max_inf_test( ) );

static_assert( daw::min( 5, 3, 100, 3, 4, -100, 0xFFFF ) == -100 );

static_assert( daw::math::nearly_equal(
  daw::min( 5, 3, 100, std::numeric_limits<float>::infinity( ), 3, 4, -100,
            0xFFFF ),
  -100.0f ) );
int main( ) {
	daw_math_factorial( );
	daw_max_inf_test( );
}
