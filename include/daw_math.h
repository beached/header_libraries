// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace daw {
	namespace math {
		template<typename T>
		constexpr T const PI = T( 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899 );

		template<class T, class U>
		T round_to_nearest( const T &value, const U &rnd_by ) {
			static_assert( std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type" );
			static_assert( std::is_floating_point<U>::value,
			               "Second template parameter must be a floating point type" );
			const auto rnd = std::round( static_cast<U>( value ) / rnd_by );
			const auto ret = rnd * rnd_by;
			return static_cast<T>( ret );
		}

		template<class T, class U>
		T floor_by( const T &value, const U &rnd_by ) {
			static_assert( std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type" );
			static_assert( std::is_floating_point<U>::value,
			               "Second template parameter must be a floating point type" );
			const auto rnd = std::floor( static_cast<U>( value ) / rnd_by );
			const auto ret = rnd * rnd_by;
			assert(
			    ret <=
			    value ); // , __func__": Error, return value should always be less than or equal to value supplied" );
			return static_cast<T>( ret );
		}

		template<class T, class U>
		T ceil_by( const T &value, const U &rnd_by ) {
			static_assert( std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type" );
			static_assert( std::is_floating_point<U>::value,
			               "Second template parameter must be a floating point type" );
			const auto rnd = std::ceil( static_cast<U>( value ) / rnd_by );
			const auto ret = rnd * rnd_by;
			assert( ret >= value ); // , __func__": Error, return value should always be greater than or equal to value
			                        // supplied" );
			return static_cast<T>( ret );
		}

		namespace impl {
			template<typename T>
			constexpr T abs( T t ) {
				return t >= 0 ? t : -t;
			}

			template<typename T>
			constexpr uintmax_t factorial_impl( T t ) {
				return t <= 1 ? 1 : factorial_impl( t - 1 );
			}

		} // namespace impl

		template<typename T>
		constexpr uintmax_t factorial( T t ) {
			return t == 0 ? 1
			              : t < 0 ? factorial( static_cast<uintmax_t>( impl::abs( t ) ) ) : impl::factorial_impl( t );
		}

		namespace impl {
			template<typename T>
			constexpr T pow_impl( T base, size_t exponent ) {
				for( size_t k = 0; k < exponent; ++k ) {
					base *= base;
				}
				return base;
			}

		} // namespace impl

		template<typename T>
		constexpr T pow( T base, size_t exponent ) {
			return base == 0 ? 0 : exponent > 0 ? impl::pow_impl( base, exponent ) : 1;
		}

		namespace impl {
			struct constants {
				constexpr static double const tol = 0.001;
			};

			template<typename T>
			constexpr auto cube( T x ) {
				return x * x * x;
			}

			// Based on the triple-angle formula: sin 3x = 3 sin x - 4 sin ^3 x
			template<typename R>
			constexpr R sin_helper( R x ) {
				return x < constants::tol ? x
				                          : static_cast<R>( 3.0 ) * ( sin_helper( x / static_cast<R>( 3.0 ) ) ) -
				                                static_cast<R>( 4.0 ) * cube( sin_helper( x / static_cast<R>( 3.0 ) ) );
			}
		} // namespace impl

		template<typename R>
		constexpr R sin( R x ) {
			return impl::sin_helper( x < 0 ? PI<R> - x : x );
		}

		template<typename R>
		constexpr R cos( R x ) {
			if( 0.0 == x ) {
				return 1.0;
			}
			return sin( ( PI<R> / 2.0 ) - x );
		}

		template<class T>
		auto sqr( T const &value ) {
			static_assert( std::is_arithmetic<T>::value, "Template parameter must be an arithmetic type" );
			return value * value;
		}

		///
		/// <summary>Set the minimum value allowed</summary>
		///
		template<typename T, typename U>
		auto value_or_min( T const &value, U const &min_value ) {
			if( min_value > value ) {
				return min_value;
			}
			return value;
		}

	} // namespace math
} // namespace daw
