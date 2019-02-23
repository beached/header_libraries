// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <cmath>
#include <iostream>
#include <limits>

#include "daw/daw_benchmark.h"
#include "daw/daw_cxmath.h"
#include "daw/daw_random.h"

static_assert( daw::math::math_impl::bits( 2.0f ).raw_value( ) ==
               0x4000'0000U );
static_assert( daw::math::math_impl::bits( 234324.34375f ).raw_value( ) ==
               0x4864'd516U );
static_assert( daw::math::math_impl::bits( -1.99999988079071044921875f )
                 .raw_value( ) == 0xbfff'ffffU );
static_assert( daw::math::math_impl::bits( 0.0f ).raw_value( ) ==
               0x0000'0000U );

static_assert( daw::math::sqrt( 4.0f ) == 2.0f );
static_assert( daw::math::copy_sign( 2.0f, 1 ) == 2.0f );
static_assert( daw::math::copy_sign( 2.0f, -1 ) == -2.0f );
static_assert( daw::math::copy_sign( -2.0f, -1 ) == -2.0f );
static_assert( daw::math::copy_sign( -2.0f, 1 ) == 2.0f );
static_assert( daw::math::math_impl::pow2( -1 ) == 0.5f );
static_assert( daw::math::math_impl::pow2( -2 ) == 0.25f );
static_assert( daw::math::math_impl::pow2( 1 ) == 2.0f );
static_assert( daw::math::math_impl::pow2( 2 ) == 4.0f );

void out_sqrt( float f ) {
	auto result = daw::math::sqrt( f );
	auto E = ( daw::math::sqrt( f * f ) - f ) / f;
	auto cm = std::sqrt( f );
	auto E2 = ( std::sqrt( f * f ) - f ) / f;

	std::cout.precision( std::numeric_limits<float>::max_digits10 );
	std::cout << f << "-> (" << result << ", " << cm << ") E: (" << E << ", "
	          << E2 << ")\n";
}

int main( ) {
	std::cout.precision( std::numeric_limits<float>::max_digits10 );
	out_sqrt( -1.0f );
	out_sqrt( 0.1f );
	out_sqrt( 0.5f );
	out_sqrt( 0.01f );
	out_sqrt( 2.0f );
	out_sqrt( 3.0f );
	out_sqrt( 4.0f );
	out_sqrt( 5.0f );
	out_sqrt( 27.0f );
	out_sqrt( 64.0f );
	out_sqrt( 100.0f );
	out_sqrt( std::numeric_limits<float>::min( ) );
	out_sqrt( std::numeric_limits<float>::max( ) );
	out_sqrt( std::numeric_limits<float>::infinity( ) );
	out_sqrt( -std::numeric_limits<float>::infinity( ) );
	out_sqrt( std::numeric_limits<float>::quiet_NaN( ) );

	auto const nums =
	  daw::make_random_data<int32_t, std::vector<float>>( 1'000, -1'000, 1'000 );

	daw::bench_n_test<100'000>(
	  "daw::math::math_impl::bits",
	  []( auto &&floats ) {
		  float sum = 0.0f;
		  for( auto num : floats ) {
			  sum += daw::math::math_impl::bits( num ).raw_value( );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  nums );
	daw::bench_n_test<100'000>( "daw::math::math_impl::fexp",
	                            []( auto &&floats ) {
		                            intmax_t sum = 0.0f;
		                            for( auto num : floats ) {
			                            sum += *daw::math::math_impl::fexp( num );
		                            }
		                            daw::do_not_optimize( sum );
		                            return sum;
	                            },
	                            nums );
	daw::bench_n_test<100'000>( "daw::math::sqrt",
	                            []( auto &&floats ) {
		                            float sum = 0.0f;
		                            for( auto num : floats ) {
			                            sum += daw::math::sqrt( num );
		                            }
		                            daw::do_not_optimize( sum );
		                            return sum;
	                            },
	                            nums );
	daw::bench_n_test<100'000>( "std::sqrt",
	                            []( auto &&floats ) {
		                            float sum = 0.0f;
		                            for( auto num : floats ) {
			                            sum += std::sqrt( num );
		                            }
		                            daw::do_not_optimize( sum );
		                            return sum;
	                            },
	                            nums );

	return 0;
}
