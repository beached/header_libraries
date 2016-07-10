// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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
#endif	//max

#include <cassert>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <cmath>
#include <vector>

namespace daw {
	namespace math {
		template<typename T>
		constexpr T const PI = T(3.14159265358979323846264338327950288419716939937510582097494459230781640628620899);
	
		template<class T, class U>
		T round_to_nearest( const T& value, const U& rnd_by ) {
			static_assert(std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type");
			static_assert(std::is_floating_point<U>::value, "Second template parameter must be a floating point type");
			const auto rnd = std::round( static_cast<U>(value) / rnd_by );
			const auto ret = rnd*rnd_by;
			return static_cast<T>(ret);
		}

		template<class T, class U>
		T floor_by( const T& value, const U& rnd_by ) {
			static_assert(std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type");
			static_assert(std::is_floating_point<U>::value, "Second template parameter must be a floating point type");
			const auto rnd = std::floor( static_cast<U>(value) / rnd_by );
			const auto ret = rnd*rnd_by;
			assert( ret <= value );// , __func__": Error, return value should always be less than or equal to value supplied" );
			return static_cast<T>(ret);
		}

		template<class T, class U>
		T ceil_by( const T& value, const U& rnd_by ) {
			static_assert(std::is_arithmetic<T>::value, "First template parameter must be an arithmetic type");
			static_assert(std::is_floating_point<U>::value, "Second template parameter must be a floating point type");
			const auto rnd = std::ceil( static_cast<U>(value) / rnd_by );
			const auto ret = rnd*rnd_by;
			assert( ret>= value ); // , __func__": Error, return value should always be greater than or equal to value supplied" );
			return static_cast<T>(ret);
		}

		constexpr uintmax_t factorial( uintmax_t t ) {
			if( 0 == t ) {
				return 1;
			}
			uintmax_t result = t--;
			for( ; t > 0; --t ) {
				result *= t;	
			}
			return result;
		}
		
		constexpr double pow_DI( double base, size_t exponent ) {
			if( 0.0 == base ) {
				return 0.0;
			}
			if( exponent > 0 ) {
				for( size_t k = 0; k<exponent; ++k ) {
					base *= base;
				}
			} else {
				base = 1.0;
			}
			return base;
		}	

		namespace impl {
			struct is_64bit {
				using type = bool;
				constexpr static const bool value = sizeof( size_t ) == 2*sizeof( uint32_t );
			};
		}

		constexpr double const_cos( double rad ) {
			if( rad < -PI<double>/2.0 || rad > PI<double>/2.0 ) {
				if( rad < 0.0 ) {
					return -const_cos( rad + PI<double> );
				}
				return -const_cos( rad - PI<double> );
			}
			double rad_sqr = rad*rad;
			double numerator = rad_sqr ;
			double denominator = 2.0;	
			uintmax_t factorial = 2;
			double sum = 1.0 - (numerator/denominator);
			numerator *= rad_sqr;
			denominator *= static_cast<double>(factorial+1) * static_cast<double>(factorial+2);
			factorial += 2;
			sum = sum + (numerator/denominator);
			for( size_t n=0; n<4; ++n ) {
				numerator *= rad_sqr;
				denominator *= static_cast<double>(factorial+1) * static_cast<double>(factorial+2);
				sum = sum - (numerator/denominator);
				numerator *= rad_sqr;
				denominator *= static_cast<double>(factorial+1) * static_cast<double>(factorial+2);
				factorial += 2;
				sum = sum + (numerator/denominator);
			}
			return sum;
		}
	}	// namespace math
}	// namespace daw

