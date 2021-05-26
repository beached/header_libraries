// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_cxmath.h"
#include "daw/daw_random.h"
#include "daw/daw_uint_buffer_ios.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

template<bool ForceBitCast = false, typename Float>
constexpr bool flt_eql_exact( Float lhs, Float rhs ) {
#if defined( DAW_CX_BIT_CAST )
	using uint_t = daw::cxmath::cxmath_impl::unsigned_float_type_t<Float>;
	if( DAW_BIT_CAST( uint_t, lhs ) == DAW_BIT_CAST( uint_t, rhs ) ) {
		return true;
	}
	// This ensures the arguments show up in compiler output
	throw lhs;
#else
	if constexpr( ForceBitCast ) {
		using uint_t = daw::cxmath::cxmath_impl::unsigned_float_type_t<Float>;
		return DAW_BIT_CAST( uint_t, lhs ) == DAW_BIT_CAST( uint_t, rhs );
	} else {
		// Gets rid of warnings for things we know
		return !( lhs < rhs ) and !( rhs < lhs );
	}
#endif
}

static_assert( daw::cxmath::cxmath_impl::bits( 2.0f ).raw_value( ) ==
               0x4000'0000U );
static_assert( daw::cxmath::cxmath_impl::bits( 234324.34375f ).raw_value( ) ==
               0x4864'd516U );
static_assert( daw::cxmath::cxmath_impl::bits( -1.99999988079071044921875f )
                 .raw_value( ) == 0xbfff'ffffU );
static_assert( daw::cxmath::cxmath_impl::bits( 0.0f ).raw_value( ) ==
               0x0000'0000U );
static_assert( flt_eql_exact( daw::cxmath::sqrt( 16.0f ), 4.0f ) );
static_assert( flt_eql_exact( daw::cxmath::sqrt( 4.0f ), 2.0f ) );
static_assert( flt_eql_exact( daw::cxmath::copy_sign( 2.0f, 1.0f ), 2.0f ) );
static_assert( flt_eql_exact( daw::cxmath::copy_sign( 2.0f, -1.0f ), -2.0f ) );
static_assert( flt_eql_exact( daw::cxmath::copy_sign( -2.0f, -1.0f ), -2.0f ) );
static_assert( flt_eql_exact( daw::cxmath::copy_sign( -2.0f, 1.0f ), 2.0f ) );
static_assert( flt_eql_exact( daw::cxmath::fpow2( -1 ), 0.5f ) );
static_assert( flt_eql_exact( daw::cxmath::fpow2( -2 ), 0.25f ) );
static_assert( flt_eql_exact( daw::cxmath::fpow2( 1 ), 2.0f ) );
static_assert( flt_eql_exact( daw::cxmath::fpow2( 2 ), 4.0f ) );
static_assert( flt_eql_exact( daw::cxmath::dpow2( 0 ), 1.0 ) );
static_assert( daw::cxmath::pow10_v<5> == 100000 );

template<typename Float>
constexpr auto ulp_diff( Float lhs, Float rhs ) {
	using UInt = daw::cxmath::cxmath_impl::unsigned_float_type_t<Float>;
	UInt lhs_int = DAW_BIT_CAST( UInt, lhs );
	UInt rhs_int = DAW_BIT_CAST( UInt, rhs );
	if( lhs_int > rhs_int ) {
		return lhs_int - rhs_int;
	}
	return rhs_int - lhs_int;
}

template<typename Float>
auto compare_sqrt( Float f ) {
	auto lib_sqrt = daw::cxmath::sqrt( f );
	auto std_sqrt = std::sqrt( f );

	return ulp_diff( lib_sqrt, std_sqrt );
}

template<typename Float>
void test_sqrt( Float f ) {
	using UInt = daw::cxmath::cxmath_impl::unsigned_float_type_t<Float>;
	auto d0 = compare_sqrt( f );
	if( d0 > 0 ) {
		auto ssqrt = std::sqrt( f );
		auto lsqrt = daw::cxmath::sqrt( f );
		auto absdiff = std::abs( ssqrt - lsqrt );
		std::cerr << std::setprecision( std::numeric_limits<Float>::max_digits10 )
		          << "A ulp difference of " << d0 << " was found for the sqrt of ("
		          << f << "); std::sqrt->" << ssqrt << "("
		          << DAW_BIT_CAST( UInt, ssqrt ) << ")"
		          << " << libsqrt->" << lsqrt << "(" << DAW_BIT_CAST( UInt, lsqrt )
		          << ")\n";
	}
}
template<typename Float>
inline constexpr Float pi =
  3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446;

void sqrt_tests( ) {
	std::cout << "Testing sqrt against std::sqrt\n";
	std::cout << "*******************************\n";
#if defined( DAW_CX_BIT_CAST )
	std::cout << "double\n";
	test_sqrt( std::numeric_limits<double>::max( ) );
	test_sqrt( std::numeric_limits<double>::max( ) - 1.0f );
	test_sqrt( std::numeric_limits<double>::max( ) + 1.0f );
	test_sqrt( 0.0f + std::numeric_limits<double>::epsilon( ) );
	test_sqrt( pi<double> );
#endif
	std::cout << "float\n";
	test_sqrt( std::numeric_limits<float>::max( ) );
	test_sqrt( std::numeric_limits<float>::max( ) - 1.0f );
	test_sqrt( std::numeric_limits<float>::max( ) + 1.0f );
	test_sqrt( 0.0f + std::numeric_limits<float>::epsilon( ) );
	test_sqrt( pi<float> );
	std::cout << "*******************************\n";
}

void out_sqrt( float f ) {
	auto result = daw::cxmath::sqrt( f );
	auto E = ( daw::cxmath::sqrt( f * f ) - f ) / f;
	auto cm = std::sqrt( f );
	auto E2 = ( std::sqrt( f * f ) - f ) / f;
	auto diff = result - cm;

	std::cout.precision( std::numeric_limits<float>::max_digits10 );
	std::cout << f << "-> (" << result << ", " << cm << ") E: (" << E << ", "
	          << E2 << ") diff: " << diff << '\n';
}

int main( ) {
	sqrt_tests( );
	std::cout << "pow10( -1 ) -> " << daw::cxmath::dpow10( -1 ) << '\n';
	std::cout << "pow10( -2 ) -> " << daw::cxmath::dpow10( -2 ) << '\n';
	std::cout << "pow10( -3 ) -> " << daw::cxmath::dpow10( -3 ) << '\n';
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
	auto const dnums =
	  daw::make_random_data<int32_t, std::vector<double>>( 1'000, -1'000, 1'000 );
#if defined( DEBUG ) or not defined( NDEBUG )
	constexpr size_t RUNCOUNT = 10'000;
#else
	constexpr size_t RUNCOUNT = 100'000;
#endif
	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::cxmath_impl::bits( flt )",
	  []( auto &&floats ) {
		  float sum = 0.0f;
		  for( auto num : floats ) {
			  sum += daw::cxmath::cxmath_impl::bits( num ).raw_value( );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  nums );

	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::fpow2( int )",
	  []( auto &&floats ) {
		  float sum = 0.0;
		  for( auto num : floats ) {
			  sum += daw::cxmath::fpow2( static_cast<int32_t>( num ) );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  daw::make_random_data<int32_t, std::vector<float>>(
	    1'000, std::numeric_limits<float>::min_exponent10,
	    std::numeric_limits<float>::max_exponent10 ) );

	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::intxp( flt )",
	  []( auto &&floats ) {
		  intmax_t sum = 0.0f;
		  for( auto num : floats ) {
			  sum += *daw::cxmath::intxp( num );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  nums );
	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::set_exponent( flt, 0 )",
	  []( auto &&floats ) {
		  float sum = 0.0f;
		  for( auto num : floats ) {
			  sum += daw::cxmath::set_exponent( num, 0 );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  nums );
	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::sqrt_fast( flt )",
	  []( auto &&floats ) {
		  float sum = 0.0f;
		  for( auto num : floats ) {
			  sum += daw::cxmath::sqrt_fast( num );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  nums );
	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::sqrt( flt )",
	  []( auto &&floats ) {
		  float sum = 0.0f;
		  for( auto num : floats ) {
			  sum += daw::cxmath::sqrt( num );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  nums );
	daw::bench_n_test<RUNCOUNT>(
	  "std::sqrt( flt )",
	  []( auto &&floats ) {
		  float sum = 0.0f;
		  for( auto num : floats ) {
			  sum += std::sqrt( num );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  nums );
#if defined( DAW_CX_BIT_CAST )
	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::sqrt_fast( dbl )",
	  []( auto &&dbls ) {
		  double sum = 0.0;
		  for( auto num : dbls ) {
			  sum += daw::cxmath::sqrt_fast( num );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  dnums );
	daw::bench_n_test<RUNCOUNT>(
	  "daw::cxmath::sqrt( dbl )",
	  []( auto &&dbls ) {
		  double sum = 0.0;
		  for( auto num : dbls ) {
			  sum += daw::cxmath::sqrt( num );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  dnums );
	daw::bench_n_test<RUNCOUNT>(
	  "std::sqrt( dbl )",
	  []( auto &&dbls ) {
		  double sum = 0.0;
		  for( auto num : dbls ) {
			  sum += std::sqrt( num );
		  }
		  daw::do_not_optimize( sum );
		  return sum;
	  },
	  dnums );
#endif
	return 0;
}
