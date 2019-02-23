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

#pragma once

#include <cstdint>
#include <limits>
#include <optional>

#include "daw_do_n.h"
#include "daw_enable_if.h"

namespace daw {
	namespace math {
		constexpr std::optional<int16_t> exp2( float const f ) noexcept;
		constexpr float fpow2( int32_t exp ) noexcept;

		namespace math_impl {
			template<typename Float>
			inline constexpr auto sqrt2 = static_cast<Float>(
			  1.4142135623730950488016887242096980785696718753769480L );

			template<typename Float>
			inline constexpr auto sqrt0_5 = static_cast<Float>(
			  0.7071067811865475244008443621048490392848359376884740L );

			// Based on code from
			// https://graphics.stanford.edu/~seander/bithacks.html
			constexpr uint32_t count_leading_zeroes( uint64_t v ) noexcept {
				char const bit_position[64] = {
				  0,  1,  2,  7,  3,  13, 8,  19, 4,  25, 14, 28, 9,  34, 20, 40,
				  5,  17, 26, 38, 15, 46, 29, 48, 10, 31, 35, 54, 21, 50, 41, 57,
				  63, 6,  12, 18, 24, 27, 33, 39, 16, 37, 45, 47, 30, 53, 49, 56,
				  62, 11, 23, 32, 36, 44, 52, 55, 61, 22, 43, 51, 60, 42, 59, 58};

				v |= v >> 1; // first round down to one less than a power of 2
				v |= v >> 2;
				v |= v >> 4;
				v |= v >> 8;
				v |= v >> 16;
				v |= v >> 32;
				v = ( v >> 1 ) + 1;

				return 63U -
				       static_cast<uint32_t>(
				         bit_position[( v * 0x021'8a39'2cd3'd5dbf ) >> 58U] ); // [3]
			}

			constexpr float pow( float b, int32_t exp ) noexcept {
				auto result = 1.0f;
				while( exp < 0 ) {
					result /= b;
					exp++;
				}
				while( exp > 0 ) {
					result *= b;
					exp--;
				}
				return result;
			}

			class float_parts_t {
				uint32_t m_raw_value{};
				float m_float_value{};

			public:
				static constexpr uint32_t const PosInf = 0x7F80'0000;
				static constexpr uint32_t const NegInf = 0xFF80'0000;
				static constexpr uint32_t const NaN = 0x7FC0'0000;

				constexpr float_parts_t( uint32_t i, float f ) noexcept
				  : m_raw_value( i )
				  , m_float_value( f ) {}

				constexpr uint32_t raw_value( ) const noexcept {
					return m_raw_value;
				}

				constexpr float float_value( ) const noexcept {
					return m_float_value;
				}

				constexpr bool sign_bit( ) const noexcept {
					return ( m_raw_value >> 31U ) == 0U; // (-1)^S  0=pos, 1=neg
				}

				constexpr bool is_positive( ) const noexcept {
					return sign_bit( );
				}

				constexpr bool is_negative( ) const noexcept {
					return !sign_bit( );
				}

				constexpr uint8_t raw_exponent( ) const noexcept {
					return ( 0b0111'1111'1000'0000'0000'0000'0000'0000 & m_raw_value ) >>
					       23U;
				}

				constexpr int16_t exponent( ) const noexcept {
					int16_t const bias = 127;
					return static_cast<int16_t>( raw_exponent( ) ) - bias;
				}

				constexpr uint32_t raw_significand( ) const noexcept {
					return 0b0000'0000'0111'1111'1111'1111'1111'1111 & m_raw_value;
				}

				constexpr float significand( ) const noexcept {
					float result = m_float_value;
					if( m_float_value < 0.0f ) {
						result = -result;
					}
					auto const e = exponent( );
					if( e < 0 ) {
						return result * ::daw::math::fpow2( -e );
					}
					return result / ::daw::math::fpow2( -e );
				}

				constexpr bool is_pos_inf( ) const noexcept {
					return m_raw_value == PosInf;
				}

				constexpr bool is_neg_inf( ) const noexcept {
					return m_raw_value == NegInf;
				}

				constexpr bool is_inf( ) const noexcept {
					return is_pos_inf( ) or is_neg_inf( );
				}

				constexpr bool is_nan( ) const noexcept {
					return m_raw_value == NaN;
				}
			};

			// From: http://brnz.org/hbr/?p=1518
			constexpr float_parts_t bits( float const f ) noexcept {
				// Once c++20 use bit_cast
				if( f == 0.0f ) {
					return {0, f}; // also matches -0.0f and gives wrong result
				} else if( f > std::numeric_limits<float>::max( ) ) {
					// infinity
					return {0x7f80'0000, f};
				} else if( f < -std::numeric_limits<float>::max( ) ) {
					// negative infinity
					return {0xff800000, f};
				} else if( f != f ) {
					// NaN
					return {0x7fc0'0000, f};
				}
				bool sign = f < 0.0f;
				float abs_f = sign ? -f : f;
				int32_t exponent = 254;

				while( abs_f < 0x1p87f ) {
					abs_f *= 0x1p41f;
					exponent -= 41;
				}

				auto const a = static_cast<uint64_t>( abs_f * 0x1p-64f );
				auto lz = static_cast<int32_t>( count_leading_zeroes( a ) );
				exponent -= lz;

				if( exponent <= 0 ) {
					exponent = 0;
					lz = 8 - 1;
				}

				uint32_t significand = ( a << ( lz + 1 ) ) >> ( 64 - 23 ); // [3]
				return {( static_cast<uint32_t>( sign ? 1U : 0U ) << 31U ) |
				          ( static_cast<uint32_t>( exponent ) << 23U ) | significand,
				        f};
			}

			template<size_t iterations>
			constexpr float sqrt_newt( float const f ) noexcept {
				auto y = 0.41731f + 0.59016f * f;
				daw::algorithm::do_n<iterations>(
				  [&]( ) { y = 0.5f * ( y + ( f / y ) ); } );
				return y;
			}
		} // namespace math_impl

		constexpr float fexp2( float X, int16_t exponent ) noexcept {
			auto const exp_diff = exponent - *exp2( X );
			if( exp_diff > 0 ) {
				return fpow2( exp_diff ) * X;
			}
			return X / fpow2( -exp_diff );
		}

		constexpr float fpow2( int32_t exp ) noexcept {
			if( exp >= 0 ) {
				return static_cast<float>( 1ULL << static_cast<uint64_t>( exp ) );
			}
			float result = 1.0f;
			while( exp++ < 0 ) {
				result *= 0.5f;
			}
			return result;
		}

		constexpr std::optional<int16_t> exp2( float const f ) noexcept {
			// Once c++20 use bit_cast
			if( f == 0.0f ) {
				return 0;
			} else if( f > std::numeric_limits<float>::max( ) ) {
				return std::nullopt;
			} else if( f < -std::numeric_limits<float>::max( ) ) {
				return std::nullopt;
			} else if( f != f ) {
				return std::nullopt;
			}
			int32_t exponent = 254;
			float abs_f = f < 0 ? -f : f;

			while( abs_f < 0x1p87f ) {
				abs_f *= 0x1p41f;
				exponent -= 41;
			}

			auto const a = static_cast<uint64_t>( abs_f * 0x1p-64f );
			auto lz = static_cast<int32_t>( math_impl::count_leading_zeroes( a ) );
			exponent -= lz;

			if( exponent >= 0 ) {
				return exponent - 127;
			}
			// return -127;
			return std::nullopt;
		}

		template<typename Integer,
		         daw::enable_if_t<std::is_integral_v<Integer>> = nullptr>
		constexpr bool is_odd( Integer i ) noexcept {
			return ( static_cast<uint32_t>( i ) & 1U ) == 1U;
		}

		template<typename Integer,
		         daw::enable_if_t<std::is_integral_v<Integer>> = nullptr>
		constexpr bool is_even( Integer i ) noexcept {
			return ( static_cast<uint32_t>( i ) & 1U ) == 0U;
		}

		template<typename Float,
		         daw::enable_if_t<std::is_floating_point_v<Float>> = nullptr>
		constexpr Float abs( Float f ) noexcept {
			if( f < 0.0f ) {
				return -f;
			}
			return f;
		}

		constexpr float sqrt( float const x ) noexcept {
			size_t const iterations = 3;
			if( x < 0.0f ) {
				return std::numeric_limits<float>::quiet_NaN( );
			}
			auto const exp = exp2( x );
			if( !exp ) {
				return x;
			}
			auto const N = *exp;
			auto const f = fexp2( x, 0 );

			if( is_odd( N ) ) {
				auto const y = math_impl::sqrt_newt<iterations>( f ) / math_impl::sqrt2<float>;
				return y * fpow2( ( N + 1 ) / 2 );
			}
			return math_impl::sqrt_newt<iterations>( f ) * fpow2( N / 2 );
		}

		template<typename Number, typename Number2,
		         daw::enable_if_t<std::is_arithmetic_v<Number>,
		                          std::is_arithmetic_v<Number2>> = nullptr>
		constexpr Number copy_sign( Number x, Number2 s ) noexcept {
			if( s < 0 ) {
				if( x < 0 ) {
					return x;
				}
				return -x;
			}
			if( x < 0 ) {
				return -x;
			}
			return x;
		}

		template<typename Number,
		         daw::enable_if_t<std::is_signed_v<Number>> = nullptr>
		constexpr bool signbit( Number n ) noexcept {
			return n < 0;
		}

		template<typename Number,
		         daw::enable_if_t<!std::is_signed_v<Number>> = nullptr>
		constexpr bool signbit( Number n ) noexcept {
			return false;
		}
	} // namespace math
} // namespace daw
