// The MIT License (MIT)
//
// Copyright (c) 2014-2018 Darrell Wright
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

#pragma once
#ifdef max
#undef max
#endif // max

#include <cassert>
#include <cfenv>
#include <cmath>
#include <limits>
#include <memory>
#include <utility>

#include "daw/cpp_17.h"
#include "daw/daw_algorithm.h"
#include "daw/daw_endian.h"
#include "daw/daw_scope_guard.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"

namespace daw {
	template<typename T>
	constexpr T max( T &&t ) noexcept {
		return std::forward<T>( t );
	}

	template<typename T0, typename T1>
	constexpr auto max( T0 &&val1, T1 &&val2 ) noexcept
	  -> std::common_type_t<T0, T1> {
		if( val1 > val2 ) {
			return std::forward<T0>( val1 );
		}
		return std::forward<T1>( val2 );
	}

	template<typename T0, typename T1, typename... Ts,
	         std::enable_if_t<( sizeof...( Ts ) != 0 ), std::nullptr_t> = nullptr>
	constexpr auto max( T0 &&val1, T1 &&val2, Ts &&... vs ) noexcept
	  -> std::common_type_t<T0, T1, Ts...> {
		auto tmp = max( std::forward<Ts>( vs )... );
		if( val1 > val2 ) {
			if( val1 > tmp ) {
				return std::forward<T0>( val1 );
			}
			return tmp;
		} else if( val2 > tmp ) {
			return std::forward<T1>( val2 );
		}
		return tmp;
	}

	template<typename T>
	constexpr T min( T &&t ) noexcept {
		return std::forward<T>( t );
	}

	template<typename T0, typename T1>
	constexpr auto min( T0 &&val1, T1 &&val2 ) noexcept
	  -> std::common_type_t<T0, T1> {
		if( val1 < val2 ) {
			return std::forward<T0>( val1 );
		}
		return std::forward<T1>( val2 );
	}

	template<typename T0, typename T1, typename... Ts,
	         std::enable_if_t<( sizeof...( Ts ) != 0 ), std::nullptr_t> = nullptr>
	constexpr auto min( T0 &&val1, T1 &&val2, Ts &&... vs ) noexcept
	  -> std::common_type_t<T0, T1, Ts...> {
		auto tmp = min( std::forward<Ts>( vs )... );
		if( val1 < val2 ) {
			if( val1 < tmp ) {
				return std::forward<T0>( val1 );
			}
			return tmp;
		} else if( val2 < tmp ) {
			return std::forward<T1>( val2 );
		}
		return tmp;
	}

	namespace math {
		template<typename Float>
		struct int_for_float {
			using type = intmax_t;
		};

		template<>
		struct int_for_float<float> {
			using type = int32_t;
		};

		template<typename Float>
		using int_for_float_t = typename int_for_float<Float>::type;

		template<typename T>
		constexpr T const PI = T(
		  3.14159265358979323846264338327950288419716939937510582097494459230781640628620899 );

		template<typename Result = int_for_float_t<float>>
		constexpr Result round( float d ) noexcept {
			static_assert( daw::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( daw::is_floating_point_v<float>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d + 0.5 );
		}

		template<typename Result = int_for_float_t<double>>
		constexpr Result round( double d ) noexcept {
			static_assert( daw::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( daw::is_floating_point_v<double>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d + 0.5 );
		}

		template<typename Result = int_for_float_t<float>>
		constexpr Result floor( float d ) noexcept {
			static_assert( daw::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( daw::is_floating_point_v<float>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d );
		}

		template<typename Result = int_for_float_t<double>>
		constexpr Result floor( double d ) noexcept {
			static_assert( daw::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( daw::is_floating_point_v<double>,
			               "Argument must be a doubleing point type" );
			return static_cast<Result>( d );
		}

		template<typename Result = int_for_float_t<float>>
		constexpr Result ceil( float d ) noexcept {
			static_assert( daw::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( daw::is_floating_point_v<float>,
			               "Argument must be a floating point type" );
			return static_cast<Result>( d + 1.0 );
		}

		template<typename Result = int_for_float_t<double>>
		constexpr Result ceil( double d ) noexcept {
			static_assert( daw::is_integral_v<Result>,
			               "Result type must be integral" );
			static_assert( daw::is_floating_point_v<double>,
			               "Argument must be a doubleing point type" );
			return static_cast<Result>( d + 1.0 );
		}

		template<typename T>
		constexpr T round_by( T const value, double const rnd_by ) noexcept {
			static_assert( is_arithmetic_v<T>, "value must be an arithmetic type" );
			auto const rnd =
			  static_cast<double>( round( static_cast<double>( value ) / rnd_by ) );
			return static_cast<T>( rnd * rnd_by );
		}

		template<typename T>
		constexpr T floor_by( T const value, double const rnd_by ) noexcept {
			static_assert( is_arithmetic_v<T>, "value must be an arithmetic type" );
			auto const rnd =
			  static_cast<double>( floor( static_cast<double>( value ) / rnd_by ) );
			return static_cast<T>( rnd * rnd_by );
		}

		template<typename T>
		constexpr T ceil_by( T const value, double const rnd_by ) noexcept {
			static_assert( is_arithmetic_v<T>, "value must be an arithmetic type" );
			auto const rnd =
			  static_cast<double>( ceil( static_cast<double>( value ) / rnd_by ) );
			return static_cast<T>( rnd * rnd_by );
		}

		template<typename T>
		constexpr T abs( T t ) noexcept {
			return t >= 0 ? t : -t;
		}

		template<typename T>
		constexpr uintmax_t factorial( T t ) noexcept {
			t = abs( t );
			uintmax_t result = 1;
			for( ; t > 0; --t ) {
				result *= static_cast<uintmax_t>( t );
			}
			return result;
		}

		namespace impl {
			template<typename T>
			constexpr T pow_impl( T base, size_t exponent ) noexcept {
				for( size_t k = 0; k < exponent; ++k ) {
					base *= base;
				}
				return base;
			}

		} // namespace impl

		template<typename T>
		constexpr T pow( T base, size_t exponent ) noexcept {
			return base == 0 ? 0
			                 : exponent > 0 ? impl::pow_impl( base, exponent ) : 1;
		}

		namespace impl {
			struct constants {
				constexpr static double const tol = 0.001;
			};

			template<typename T>
			constexpr auto cube( T x ) noexcept {
				return x * x * x;
			}

			// Based on the triple-angle formula: sin 3x = 3 sin x - 4 sin ^3 x
			template<typename R>
			constexpr R sin_helper( R x ) noexcept {
				return x < constants::tol
				         ? x
				         : static_cast<R>( 3.0 ) *
				               ( sin_helper( x / static_cast<R>( 3.0 ) ) ) -
				             static_cast<R>( 4.0 ) *
				               cube( sin_helper( x / static_cast<R>( 3.0 ) ) );
			}
		} // namespace impl

		template<typename R>
		constexpr R sin( R x ) noexcept {
			return impl::sin_helper( x < 0 ? PI<R> - x : x );
		}

		template<typename R>
		constexpr R cos( R x ) noexcept {
			if( 0.0 == x ) {
				return 1.0;
			}
			return sin( ( PI<R> / 2.0 ) - x );
		}

		template<typename T>
		constexpr auto sqr( T const &value ) noexcept {
			static_assert( is_arithmetic_v<T>,
			               "Template parameter must be an arithmetic type" );
			return value * value;
		}

		///
		/// <summary>Set the minimum value allowed</summary>
		///
		template<typename T, typename U>
		constexpr auto value_or_min( T const &value, U const &min_value ) noexcept {
			if( min_value > value ) {
				return min_value;
			}
			return value;
		}

		template<typename T, std::enable_if_t<!is_floating_point_v<T>,
		                                      std::nullptr_t> = nullptr>
		constexpr bool nearly_equal( T const &a, T const &b ) noexcept {
			return a == b;
		}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif
		template<typename T,
		         std::enable_if_t<is_floating_point_v<T>, std::nullptr_t> = nullptr>
		constexpr bool nearly_equal( T const &a, T const &b ) noexcept {
			// Code from http://floating-point-gui.de/errors/comparison/
			auto absA = std::abs( a );
			auto absB = std::abs( b );
			auto diff = std::abs( a - b );

			if( a == b ) { // shortcut, handles infinities
				return true;
			}
			if( a == 0 || b == 0 || diff < std::numeric_limits<T>::min_exponent ) {
				// a or b is zero or both are extremely close to it
				// 			// relative error is less meaningful here
				return diff < ( std::numeric_limits<T>::epsilon( ) *
				                std::numeric_limits<T>::min_exponent );
			}
			// use relative error
			return diff /
			         daw::min( ( absA + absB ), std::numeric_limits<T>::max( ) ) <
			       std::numeric_limits<T>::epsilon( );
		}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

		namespace impl {
			template<typename T>
			inline void math_check_force_underflow( T &value ) noexcept {
				if( fabs( value ) < std::numeric_limits<tmp_t>::min( ) ) {
					T tmp = value * value;
					daw::force_evaluation( tmp );
				}
			}

			constexpr double POLYNOMIAL2( double d ) noexcept {
				return ( ( ( s5 * d + s4 ) * d + s3 ) * d + s2 ) * d;
			}

			constexpr double POLYNOMIAL( double d ) noexcept {
				return POLYNOMIAL2( d ) + s1;
			}

/* The computed polynomial is a variation of the Taylor series expansion for
	 sin(a):

	 a - a^3/3! + a^5/5! - a^7/7! + a^9/9! + (1 - a^2) * da / 2

	 The constants s1, s2, s3, etc. are pre-computed values of 1/3!, 1/5! and so
	 on.  The result is returned to LHS.  */
			constexpr double taylor_sine( double xx, double a, double da ) noexcept {
				double const t = ( POLYNOMIAL( xx ) * a - 0.5 * da ) * xx + da;
				return a + t;
			}
			/* Given a number partitioned into X and DX, this function computes the
			   sine of the number by combining the sin and cos of X (as computed by a
			   variation of the Taylor series) with the values looked up from the
			   sin/cos table to get the result.  */
			template<typename Double,
			         std::enable_if_t<daw::is_convertible_v<Double, double>,
			                          std::nullptr_t> = nullptr>
			double do_sin( double x, double dx ) noexcept {
				double const xold = x;
				/* Max ULP is 0.501 if |x| < 0.126, otherwise ULP is 0.518.  */
				if( fabs( x ) < 0.126 ) {
					return taylor_sine( x*x, x, dx );
				}

				if( x <= 0 ) {
					dx = -dx;
				}

				{
					double const ux = big + fabs( x );
					x = fabs( x ) - ( ux - big );
				}

				double sn, ssn, cs, ccs;
				double const xx = x * x;

				double s = x + ( dx + x * xx * ( sn3 + xx * sn5 ) );
				double c = x * dx + xx * ( cs2 + xx * ( cs4 + xx * cs6 ) );
				SINCOS_TABLE_LOOKUP( u, sn, ssn, cs, ccs );
				double cor = ( ssn + s * ccs - sn * c ) + cs * s;

				return std::copysign( sn + cor, xold );
			}

			template<typename To, typename From>
			auto to_array( From &&from ) noexcept {
				constexpr size_t const num_values = sizeof( From ) / sizeof( To );
				static_assert( sizeof( From ) == num_values * sizeof( To ),
				               "Must have integral number of To's in From" );
			}
		} // namespace impl

		template<typename Double,
		         std::enable_if_t<daw::is_convertible_v<Double, double>,
		                          std::nullptr_t> = nullptr>
		double sin( Double d ) noexcept {
			// Assumes Little Endian
			constexpr size_t const HIGH_HALF =
			  daw::endian::native == daw::endian::little ? 1 : 0;

			constexpr size_t const LOW_HALF =
			  daw::endian::native == daw::endian::little ? 0 : 1;

			auto const restore_round = daw::on_scope_exit(
			  [rnd_mode = fegetround( )]( ) { fesetround( rnd_mode ); } );

			fesetround( FE_TONEAREST );

			uint32_t[2] as_int = daw::bit_cast<uint32_t[2], double>( d );
			uint32_t m = as_int[HIGH_HALF];
			uint32_t k = 0x7FFF'FFFF & m; // no sign

			/*--------------------------- 2^-26<|x|< 0.855469---------------------- */
			if( k < 0x3E50'0000 ) { // if x->0 =>sin(x)=x
				impl::math_check_force_underflow( x );
				return d;
			}

			/*----------------------- 0.855469  <|x|<2.426265 ----------------------*/
			if( k < 0x400368fd ) {
				auto t = hp0 - fabs( x );
				/* Max ULP is 0.51.  */
				return std::copysign( do_cos( t, hp1 ), d );
			}
		}

	} // namespace math
} // namespace daw
