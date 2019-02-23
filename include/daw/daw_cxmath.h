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

#include "daw_do_n.h"
#include "daw_enable_if.h"

namespace daw {
	namespace math {
		namespace math_impl {
			template<typename Float>
			inline constexpr auto sqrt2 = static_cast<Float>(
			  1.4142135623730950488016887242096980785696718753769480L );

			template<typename Float>
			inline constexpr auto sqrt0_5 = static_cast<Float>(
			  0.7071067811865475244008443621048490392848359376884740L );

			// Based on code from
			// https://graphics.stanford.edu/~seander/bithacks.html
			constexpr int count_leading_zeroes( uint64_t v ) noexcept {
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

				return 63 - bit_position[( v * 0x0218a392cd3d5dbf ) >> 58]; // [3]
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

				constexpr uint32_t float_value( ) const noexcept {
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
					return ( 0b01111111100000000000000000000000 & m_raw_value ) >> 23U;
				}

				constexpr int16_t exponent( ) const noexcept {
					int16_t const bias = 127;
					return static_cast<int16_t>( raw_exponent( ) ) - bias;
				}

				constexpr uint32_t raw_significand( ) const noexcept {
					return 0b00000000011111111111111111111111 & m_raw_value;
				}

				constexpr float significand( ) const noexcept {
					float result = m_float_value;
					if( m_float_value < 0.0f ) {
						result = -result;
					}
					auto const e = exponent( );
					if( e < 0 ) {
						return result * ::daw::math::math_impl::pow( 2.0f, -e );
					}
					return result / ::daw::math::math_impl::pow( 2.0f, -e );
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
				if( f == 0.0f ) {
					return {0, f}; // also matches -0.0f and gives wrong result
				} else if( f > std::numeric_limits<float>::max( ) ) {
					return {0x7f800000, f};
				} else if( f < -std::numeric_limits<float>::max( ) ) {
					return {0xff800000, f};
				} else if( f != f ) {     // NaN
					return {0x7fc00000, f}; // This is my NaN...
				}
				bool sign = f < 0.0f;
				float abs_f = sign ? -f : f;
				int16_t exponent = 254;

				while( abs_f < 0x1p87f ) {
					abs_f *= 0x1p41f;
					exponent -= 41;
				}

				auto const a = static_cast<uint64_t>( abs_f * 0x1p-64f );
				auto lz = count_leading_zeroes( a );
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

			constexpr float setxp( float X, int16_t exponent ) noexcept {
				auto const bit_parts = bits( X );
				auto const exp_diff = exponent - ( bit_parts.exponent( ) );
				if( exp_diff > 0 ) {
					return pow( 2.0f, exp_diff ) * X;
				}
				return X / pow( 2.0f, -exp_diff );
			}
		} // namespace math_impl

		template<typename Number, daw::enable_if_t<std::is_arithmetic_v<Number>> = nullptr>
		constexpr Number abs( Number n ) noexcept {
			if( n < 0 ) {
				return -n;
			}
			return n;
		};

		constexpr float sqrt( float x ) noexcept {
			size_t const iterations = 3;
			auto const parts = math_impl::bits( x );
			if( parts.is_negative( ) ) {
				return std::numeric_limits<float>::quiet_NaN( );
			}
			switch( parts.raw_value( ) ) {
			case math_impl::float_parts_t::PosInf:
			case math_impl::float_parts_t::NegInf:
			case math_impl::float_parts_t::NaN:
				return x;
			}
			auto N = math_impl::bits( x ).exponent( );
			auto f = math_impl::setxp( x, 0 );

			auto y = 0.41731f + 0.59016f * f;
			daw::algorithm::do_n<iterations>(
			  [&]( ) { y = 0.5f * ( y + ( f / y ) ); } );

			if( abs( N ) % 2 == 1 ) {
				y = y * math_impl::sqrt0_5<float>;
				++N;
			}
			return y * math_impl::pow( 2.0f, N / 2 );
		}
	} // namespace math
} // namespace daw
