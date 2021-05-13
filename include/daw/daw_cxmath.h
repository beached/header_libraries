// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_arith_traits.h"
#include "daw_assume.h"
#include "daw_bit_cast.h"
#include "daw_do_n.h"
#include "daw_enable_if.h"
#include "daw_likely.h"
#include "daw_uint_buffer.h"
#include "impl/daw_math_impl.h"

#include <array>
#include <ciso646>
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

namespace daw::cxmath {
	enum class fp_classes { normal, zero, subnormal, nan, infinity };

	namespace cxmath_impl {
		struct no_specialization {};

		template<typename>
		struct unsigned_float_type_impl;

		template<>
		struct unsigned_float_type_impl<double> {
			static_assert( daw::numeric_limits<double>::is_iec559,
			               "Only IEEE754 doubles are supported" );
			static_assert( sizeof( double ) == 8U, "Incompatible double type" );
			using type = daw::UInt64;
		};

		template<>
		struct unsigned_float_type_impl<float> {
			static_assert( daw::numeric_limits<float>::is_iec559,
			               "Only IEEE754 floats are supported" );
			static_assert( sizeof( float ) == 4U, "Incompatible float type" );
			using type = daw::UInt32;
		};

		template<typename T>
		using unsigned_float_type_t = typename unsigned_float_type_impl<T>::type;

		template<typename>
		inline constexpr no_specialization significand_mask_v = { };

		template<>
		inline constexpr daw::UInt64 significand_mask_v<double>{
		  0x000F'FFFF'FFFF'FFFFULL };

		template<>
		inline constexpr daw::UInt32 significand_mask_v<float>{ 0x007F'FFFFUL };

		template<typename T>
		inline constexpr auto not_significand_mask_v = ~significand_mask_v<T>;

		template<typename>
		inline constexpr no_specialization exponent_mask_v = { };

		template<>
		inline constexpr daw::UInt64 exponent_mask_v<double>{
		  0x7FF0'0000'0000'0000ULL };

		template<>
		inline constexpr daw::UInt32 exponent_mask_v<float>{ 0x7F80'0000UL };

		template<typename T>
		inline constexpr auto not_exponent_mask_v = ~exponent_mask_v<T>;

		template<typename>
		inline constexpr no_specialization exponent_bias_v = { };

		template<>
		inline constexpr std::int32_t exponent_bias_v<double> = 1023L;

		template<>
		inline constexpr std::int32_t exponent_bias_v<float> = 127L;

		template<typename>
		inline constexpr no_specialization exponent_start_bit_v = { };

		template<>
		inline constexpr daw::UInt64 exponent_start_bit_v<double>{ 52U };

		template<>
		inline constexpr daw::UInt32 exponent_start_bit_v<float>{ 23U };

		template<typename>
		inline constexpr no_specialization sign_start_bit_v = { };

		template<>
		inline constexpr daw::UInt64 sign_start_bit_v<double>{ 63U };

		template<>
		inline constexpr daw::UInt32 sign_start_bit_v<float>{ 31U };

		template<typename>
		inline constexpr no_specialization sign_mask_v = { };

		template<>
		inline constexpr daw::UInt64 sign_mask_v<double>{
		  0x8000'0000'0000'0000ULL };

		template<>
		inline constexpr daw::UInt32 sign_mask_v<float>{ 0x8000'0000UL };

		template<typename Real>
		inline constexpr auto not_sign_mask_v = ~sign_mask_v<Real>;

		template<typename Real, typename UInt>
		inline constexpr bool is_matching_v =
		  std::is_same_v<unsigned_float_type_t<Real>, UInt>;

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt get_significand_impl( UInt dint ) {
			return dint & significand_mask_v<Real>;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt set_significand_impl( UInt dint,
		                                                          UInt sig ) {

			dint &= not_significand_mask_v<Real>;
			dint |= sig;
			return dint;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt get_raw_exponent_impl( UInt dint ) {
			dint &= exponent_mask_v<Real>;
			return dint;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt set_raw_exponent_impl( UInt dint,
		                                                           UInt exp ) {
			dint &= not_exponent_mask_v<Real>;
			dint |= exp;
			return dint;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr std::int32_t get_exponent_impl( UInt dint ) {
			UInt raw_exp = dint & exponent_mask_v<Real>;
			return static_cast<std::int32_t>( raw_exp >>
			                                  exponent_start_bit_v<Real> ) -
			       exponent_bias_v<Real>;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt set_exponent_impl( UInt dint,
		                                                       std::int32_t exp ) {
			if( exp == 0 ) {
				return DAW_BIT_CAST( UInt, Real{ 1.0 } );
			}
			exp += exponent_bias_v<Real>;
			UInt new_exp = static_cast<UInt>( exp ) << exponent_start_bit_v<Real>;
			dint &= not_exponent_mask_v<Real>;
			dint |= new_exp;
			return dint;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt get_sign_raw_impl( UInt dint ) {
			return dint & sign_mask_v<Real>;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt set_sign_raw_impl( UInt dint,
		                                                       UInt sign ) {
			dint &= not_sign_mask_v<Real>;
			dint |= sign;
			return dint;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt get_sign_impl( UInt dint ) {
			return ( dint & sign_mask_v<Real> ) >> sign_start_bit_v<Real>;
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		[[nodiscard]] inline constexpr UInt set_sign_impl( UInt dint, UInt sign ) {
			sign <<= sign_start_bit_v<Real>;
			return set_sign_raw_impl<Real>( dint, sign );
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		inline constexpr bool is_nan_impl( UInt dint ) {
			bool const exp = (dint & exponent_mask_v<Real>) == exponent_mask_v<Real>;
			return exp and ( get_significand_impl<Real>( dint ) != 0 );
		}

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		inline constexpr bool is_zero_impl( UInt dint ) {
			return ( not is_nan_impl<Real>( dint ) ) &
			       ( get_significand_impl<Real>( dint ) == 0 );
		}

		constexpr fp_classes fp_classify_impl( daw::UInt32 fint ) {
			fint &= 0x7FFF'FFFFUL;
			if( fint == 0 ) {
				return fp_classes::zero;
			}
			if( fint < 0x80'0000UL ) {
				return fp_classes::subnormal;
			}
			if( fint > 0x7F80'0000UL ) {
				return fp_classes::nan;
			}
			if( fint == 0x7F80'0000UL ) {
				return fp_classes::infinity;
			}
			return fp_classes::normal;
		}

		constexpr fp_classes fp_classify_impl( daw::UInt64 dint ) {
			auto lx = static_cast<std::uint32_t>( dint & 0x0000'0000'FFFF'FFFFULL );
			auto hx = static_cast<std::uint32_t>( dint >> 32U );
			lx |= hx & 0x000F'FFFFUL;
			hx &= 0x7FF0'0000UL;
			if( ( hx | lx ) == 0 ) {
				return fp_classes::zero;
			}
			if( hx == 0 ) {
				return fp_classes::subnormal;
			}
			if( hx == 0x7FF0'0000UL ) {
				if( lx != 0 ) {
					return fp_classes::nan;
				}
				return fp_classes::infinity;
			}
			return fp_classes::normal;
		}

		// U32

#if defined( DAW_CX_BIT_CAST )
		[[nodiscard]] constexpr int16_t intxp2( float f ) noexcept {
			auto const ieee754float = DAW_BIT_CAST( std::uint32_t, f );
			constexpr std::uint32_t const mask_msb = 0x7FFF'FFFFU;
			return static_cast<std::int16_t>( ( ieee754float & mask_msb ) >> 23U ) -
			       127;
		}
#endif
	} // namespace cxmath_impl

	template<typename Float>
	inline constexpr auto sqrt2 = static_cast<Float>(
	  1.4142135623730950488016887242096980785696718753769480L );

	template<typename Float>
	inline constexpr auto sqrt0_5 = static_cast<Float>(
	  0.7071067811865475244008443621048490392848359376884740L );

#if defined( __has_builtin )
#if __has_builtin( __builtin_clz )
#define DAW_HAS_CLZ_BUILTIN
#endif
#endif

#if defined( DAW_HAS_CLZ_BUILTIN )
	[[nodiscard]] constexpr std::uint32_t
	count_leading_zeroes( std::uint64_t v ) noexcept {
		auto high = static_cast<std::uint32_t>( v >> 32U );
		if( high != 0 ) {
			return static_cast<std::uint32_t>( __builtin_clz( high ) );
		}
		auto low = static_cast<std::uint32_t>( v );
		if( low != 0 ) {
			return 32U + static_cast<std::uint32_t>( __builtin_clz( low ) );
		}
		return 64;
	}
#else
	// Based on code from
	// https://graphics.stanford.edu/~seander/bithacks.html
	[[nodiscard]] constexpr std::uint32_t
	count_leading_zeroes( std::uint64_t v ) noexcept {
		char const bit_position[64] = {
		  0,  1,  2,  7,  3,  13, 8,  19, 4,  25, 14, 28, 9,  34, 20, 40,
		  5,  17, 26, 38, 15, 46, 29, 48, 10, 31, 35, 54, 21, 50, 41, 57,
		  63, 6,  12, 18, 24, 27, 33, 39, 16, 37, 45, 47, 30, 53, 49, 56,
		  62, 11, 23, 32, 36, 44, 52, 55, 61, 22, 43, 51, 60, 42, 59, 58 };

		v |= v >> 1U; // first round down to one less than a power of 2
		v |= v >> 2U;
		v |= v >> 4U;
		v |= v >> 8U;
		v |= v >> 16U;
		v |= v >> 32U;
		v = ( v >> 1U ) + 1U;

		return 63U - static_cast<std::uint32_t>(
		               bit_position[( v * 0x021'8a39'2cd3'd5dbf ) >> 58U] ); // [3]
	}
#endif

	[[nodiscard]] constexpr float pow( float b, int32_t exp ) noexcept {
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

	[[nodiscard]] constexpr float setxp( float f, std::int32_t exp ) {
		auto i = DAW_BIT_CAST( std::uint32_t, f );
		i &= ~0x7F80'0000U;
		i |= ( static_cast<std::uint32_t>( 127 + exp ) & 0xFFU ) << 23U;
		return DAW_BIT_CAST( float, i );
	}

	namespace cxmath_impl {
		class float_parts_t {
			std::uint32_t m_raw_value{ };
			float m_float_value{ };

		public:
			static constexpr std::uint32_t const PosInf = 0x7F80'0000;
			static constexpr std::uint32_t const NegInf = 0xFF80'0000;
			static constexpr std::uint32_t const NaN = 0x7FC0'0000;

			constexpr float_parts_t( std::uint32_t i, float f ) noexcept
			  : m_raw_value( i )
			  , m_float_value( f ) {}

			[[nodiscard]] constexpr std::uint32_t raw_value( ) const noexcept {
				return m_raw_value;
			}

			[[nodiscard]] constexpr float float_value( ) const noexcept {
				return m_float_value;
			}

			[[nodiscard]] constexpr bool sign_bit( ) const noexcept {
				return ( m_raw_value >> 31U ) == 0U; // (-1)^S  0=pos, 1=neg
			}

			[[nodiscard]] constexpr bool is_positive( ) const noexcept {
				return sign_bit( );
			}

			[[nodiscard]] constexpr bool is_negative( ) const noexcept {
				return !sign_bit( );
			}

			[[nodiscard]] constexpr uint8_t raw_exponent( ) const noexcept {
				return static_cast<uint8_t>(
				  ( 0b0111'1111'1000'0000'0000'0000'0000'0000 & m_raw_value ) >> 23U );
			}

			[[nodiscard]] constexpr std::int16_t exponent( ) const noexcept {
				std::int16_t const bias = 127;
				return static_cast<std::int16_t>( raw_exponent( ) ) - bias;
			}

			[[nodiscard]] constexpr std::uint32_t raw_significand( ) const noexcept {
				return 0b0000'0000'0111'1111'1111'1111'1111'1111 & m_raw_value;
			}

			[[nodiscard]] constexpr float significand( ) const noexcept {
				float result = m_float_value;
				if( m_float_value < 0.0f ) {
					result = -result;
				}
				auto const e = exponent( );
				if( e < 0 ) {
					return result * setxp( 2.0f, -e );
				}
				return result / setxp( 2.0f, -e );
			}

			[[nodiscard]] constexpr bool is_pos_inf( ) const noexcept {
				return m_raw_value == PosInf;
			}

			[[nodiscard]] constexpr bool is_neg_inf( ) const noexcept {
				return m_raw_value == NegInf;
			}

			[[nodiscard]] constexpr bool is_inf( ) const noexcept {
				return is_pos_inf( ) or is_neg_inf( );
			}

			[[nodiscard]] constexpr bool is_nan( ) const noexcept {
				return m_raw_value == NaN;
			}
		};

		// From: http://brnz.org/hbr/?p=1518
		[[nodiscard]] constexpr float_parts_t bits( float const f ) noexcept {
#if defined( DAW_CX_BIT_CAST )
			static_assert( sizeof( float ) == 4U );
			return float_parts_t( DAW_BIT_CAST( std::uint32_t, f ), f );
#else
			// Once c++20 use bit_cast
			if( f == 0.0f ) {
				return { 0, f }; // also matches -0.0f and gives wrong result
			} else if( f > daw::numeric_limits<float>::max( ) ) {
				// infinity
				return { 0x7f80'0000, f };
			} else if( f < -daw::numeric_limits<float>::max( ) ) {
				// negative infinity
				return { 0xff800000, f };
			} else if( f != f ) {
				// NaN
				return { 0x7fc0'0000, f };
			}
			bool sign = f < 0.0f;
			float abs_f = sign ? -f : f;
			int32_t exponent = 254;

			while( abs_f < 0x1p87f ) {
				abs_f *= 0x1p41f;
				exponent -= 41;
			}

			auto const a = static_cast<std::uint64_t>( abs_f * 0x1p-64f );
			auto lz = static_cast<int32_t>( count_leading_zeroes( a ) );
			exponent -= lz;

			if( exponent <= 0 ) {
				exponent = 0;
				lz = 8 - 1;
			}

			std::uint32_t significand = ( a << ( lz + 1 ) ) >> ( 64 - 23 ); // [3]
			return { ( static_cast<std::uint32_t>( sign ? 1U : 0U ) << 31U ) |
			           ( static_cast<std::uint32_t>( exponent ) << 23U ) |
			           significand,
			         f };
#endif
		}

		template<typename Float>
		[[nodiscard]] constexpr Float pow2_impl2( intmax_t exp ) noexcept {
			bool is_neg = exp < 0;
			exp = is_neg ? -exp : exp;
			auto const max_shft = daw::min(
			  static_cast<size_t>( daw::numeric_limits<Float>::max_exponent10 ),
			  ( sizeof( size_t ) * 8ULL ) );
			Float result = 1.0;

			while( static_cast<size_t>( exp ) >= max_shft ) {
				result *= static_cast<Float>( daw::numeric_limits<size_t>::max( ) );
				exp -= max_shft;
			}
			if( exp > 0 ) {
				result *= static_cast<Float>( 1ULL << static_cast<size_t>( exp ) );
			}
			if( is_neg && result != 0.0 ) {
				result = static_cast<Float>( 1.0 ) / result;
			}
			return result;
		}

		[[nodiscard]] constexpr size_t pow10_impl( intmax_t exp ) noexcept {
			// exp is < floor( log10( numeric_limits<size_t>::max( ) ) )
			size_t result = 1ULL;
			while( exp-- > 0 ) {
				result *= 10ULL;
			}
			return result;
		}

		template<intmax_t exp>
		[[nodiscard]] constexpr size_t pow10_impl( ) noexcept {
			return pow10_impl( exp );
		}

		template<typename Float>
		[[nodiscard]] constexpr Float fpow10_impl2( intmax_t exp ) noexcept {
			bool is_neg = exp < 0;
			exp = is_neg ? -exp : exp;

			auto const max_spow =
			  daw::min( daw::numeric_limits<Float>::max_exponent10,
			            daw::numeric_limits<size_t>::digits10 );
			Float result = 1.0;

			while( exp >= max_spow ) {
				result *= static_cast<Float>( pow10_impl<max_spow>( ) );
				exp -= max_spow;
			}
			if( exp > 0 ) {
				result *= static_cast<Float>( pow10_impl( exp ) );
			}
			if( is_neg && result != 0.0 ) {
				result = static_cast<Float>( 1.0 ) / result;
			}
			return result;
		}

		template<typename Integer>
		[[nodiscard]] constexpr Integer pow10_impl2( intmax_t exp ) noexcept {
			Integer result = 1;
			while( exp-- > 0 ) {
				result *= 10;
			}
			return result;
		}

		template<typename Float>
		[[nodiscard]] constexpr auto calc_pow2s( ) noexcept {
			intmax_t const min_e = daw::numeric_limits<Float>::min_exponent10;
			intmax_t const max_e = daw::numeric_limits<Float>::max_exponent10;
			std::array<Float, max_e - min_e> result{ };
			intmax_t n = max_e - min_e;
			while( n-- > 0 ) {
				result[static_cast<size_t>( n )] = pow2_impl2<Float>( n + min_e );
			}
			return result;
		}

		template<typename Float>
		[[nodiscard]] constexpr auto calc_fpow10s( ) noexcept {
			intmax_t const min_e = daw::numeric_limits<Float>::min_exponent10;
			intmax_t const max_e = daw::numeric_limits<Float>::max_exponent10;
			std::array<Float, max_e - min_e> result{ };
			intmax_t n = max_e - min_e;
			while( n-- > 0 ) {
				result[static_cast<size_t>( n )] = fpow10_impl2<Float>( n + min_e );
			}
			return result;
		}

		template<typename Integer>
		[[nodiscard]] constexpr auto calc_pow10s( ) noexcept {
			std::array<Integer, daw::numeric_limits<Integer>::digits10> result{ };
			intmax_t n = daw::numeric_limits<Integer>::digits10;
			while( n-- > 0 ) {
				result[static_cast<size_t>( n )] = pow10_impl2<Integer>( n );
			}
			return result;
		}

		template<typename Float>
		class [[nodiscard]] pow2_t {
			static constexpr std::array const m_tbl = calc_pow2s<Float>( );

		public:
			template<typename Result = Float>
			[[nodiscard]] static constexpr Result get( intmax_t pos ) noexcept {
				auto const zero = static_cast<intmax_t>( m_tbl.size( ) / 2ULL );
				return static_cast<Result>( m_tbl[static_cast<size_t>( zero + pos )] );
			}
		};

		template<typename Float>
		class [[nodiscard]] fpow10_t {
			static constexpr std::array const m_tbl = calc_fpow10s<Float>( );

		public:
			template<typename Result = Float>
			[[nodiscard]] static constexpr Result get( intmax_t pos ) noexcept {
				auto const zero = static_cast<intmax_t>( m_tbl.size( ) / 2ULL );
				return static_cast<Result>( m_tbl[static_cast<size_t>( zero + pos )] );
			}
		};

		template<typename Integer>
		class [[nodiscard]] pow10_t {
			static constexpr std::array const m_tbl = calc_pow10s<Integer>( );

		public:
			template<typename Result = Integer>
			[[nodiscard]] static constexpr Result get( size_t pos ) noexcept {
				return static_cast<Result>( m_tbl[pos] );
			}
		};

		[[nodiscard]] constexpr float fexp3( float f, std::int32_t exponent,
		                                     std::int32_t old_exponent ) noexcept {
			auto const exp_diff = exponent - old_exponent;
			if( exp_diff > 0 ) {
				return setxp( 2.0f, exp_diff ) * f;
			}
			return f / setxp( 2.0f, -exp_diff );
		}
	} // namespace cxmath_impl

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr fp_classes fp_classify( Real r ) {
		auto uint = DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, r );
		return cxmath_impl::fp_classify_impl( uint );
	}

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr bool is_nan( Real r ) {
		return fp_classify( r ) == fp_classes::nan;
	}
	static_assert( is_nan( daw::numeric_limits<double>::quiet_NaN( ) ) );
	static_assert( is_nan( daw::numeric_limits<double>::signaling_NaN( ) ) );
	static_assert( not is_nan( daw::numeric_limits<double>::infinity( ) ) );
	static_assert( is_nan( daw::numeric_limits<float>::quiet_NaN( ) ) );
	static_assert( is_nan( daw::numeric_limits<float>::signaling_NaN( ) ) );
	static_assert( not is_nan( daw::numeric_limits<float>::infinity( ) ) );

	[[nodiscard]] constexpr double ldexp( double d,
	                                      std::int32_t exponent ) noexcept {
		daw::UInt64 dint = DAW_BIT_CAST( daw::UInt64, 2.0 );
		exponent += 1023;
		daw::UInt64 new_exp = static_cast<daw::UInt64>( exponent );
		constexpr daw::UInt64 remove_mask{ ~0x7FF0'0000'0000'0000ULL };
		double result =
		  DAW_BIT_CAST( double, ( dint & remove_mask ) | ( new_exp << 52U ) );
		return d * result;
	}

	template<int32_t exp>
	constexpr float fpow2_v = cxmath_impl::pow2_t<double>::get<float>( exp );

	template<int32_t exp>
	constexpr double dpow2_v = cxmath_impl::pow2_t<double>::get( exp );

	template<int32_t exp>
	constexpr float fpow10_v = cxmath_impl::fpow10_t<double>::get<float>( exp );

	template<int32_t exp>
	constexpr double dpow10_v = cxmath_impl::fpow10_t<double>::get( exp );

	template<size_t exp>
	constexpr uintmax_t pow10_v = cxmath_impl::pow10_t<uintmax_t>::get( exp );

	constexpr float fpow2( std::int32_t exp ) noexcept {
		using Real = float;
#if defined( DAW_CX_BIT_CAST )
		if( exp >= daw::numeric_limits<Real>::max_exponent ) {
			return daw::numeric_limits<Real>::infinity( );
		} else if( exp <= daw::numeric_limits<Real>::min_exponent ) {
			return 0.0;
		}
		auto result =
		  cxmath_impl::set_exponent_impl<float>( daw::UInt32{ 0 }, exp );
		return DAW_BIT_CAST( Real, result );
#else
		return cxmath_impl::pow2_t<double>::get<float>( exp );
#endif
	}
	static_assert( fpow2( 1 ) == 2.0f );
	static_assert( fpow2( 2 ) == 4.0f );

	[[nodiscard]] constexpr double dpow2( int32_t exp ) noexcept {
		return cxmath_impl::pow2_t<double>::get( exp );
	}

	[[nodiscard]] constexpr float fpow10( int32_t exp ) noexcept {
		return cxmath_impl::fpow10_t<double>::get<float>( exp );
	}

	[[nodiscard]] constexpr double dpow10( int32_t exp ) noexcept {
		return cxmath_impl::fpow10_t<double>::get( exp );
	}

	[[nodiscard]] constexpr uintmax_t pow10( size_t exp ) noexcept {
		return cxmath_impl::pow10_t<uintmax_t>::get( exp );
	}

	[[nodiscard]] constexpr float setxp( float X,
	                                     std::int16_t exponent ) noexcept {
#if defined( DAW_CX_BIT_CAST )
		static_assert( sizeof( float ) == 4 );
		auto const ieee754float = DAW_BIT_CAST( std::uint32_t, X );
		exponent += 127;
		auto const new_exp = static_cast<std::uint32_t>( exponent );
		constexpr std::uint32_t remove_mask = ~( 0xFFU << 23 );
		return DAW_BIT_CAST( float,
		                     ieee754float &( ( new_exp << 23 ) | remove_mask ) );
#else
		auto const exp_diff = exponent - *intxp( X );
		if( exp_diff > 0 ) {
			return fpow2( exp_diff ) * X;
		}
		return X / fpow2( -exp_diff );
#endif
	}

	[[nodiscard]] constexpr std::optional<std::int16_t>
	intxp( float f ) noexcept {
#if defined( DAW_CX_BIT_CAST )
		static_assert( sizeof( float ) == 4 );
		auto const ieee754float = DAW_BIT_CAST( std::uint32_t, f );
		constexpr std::uint32_t const mask_msb = 0x7FFF'FFFFU;
		return static_cast<std::int16_t>( ( ieee754float & mask_msb ) >> 23U ) -
		       127;
#else
		if( f == 0.0f ) {
			return static_cast<std::int16_t>( 0 );
		}
		if( f > daw::numeric_limits<float>::max( ) ) {
			// inf
			return std::nullopt;
		}
		if( f < -daw::numeric_limits<float>::max( ) ) {
			// -inf
			return std::nullopt;
		}
		if( f != f ) {
			// NaN
			return std::nullopt;
		}
		int32_t exponent = 254;
		float abs_f = f < 0 ? -f : f;

		while( abs_f < 0x1p87f ) {
			abs_f *= 0x1p41f;
			exponent -= 41;
		}

		auto const a = static_cast<std::uint64_t>( abs_f * 0x1p-64f );
		auto lz = static_cast<int32_t>( cxmath_impl::count_leading_zeroes( a ) );
		exponent -= lz;

		if( exponent >= 0 ) {
			return static_cast<std::int16_t>( exponent - 127 );
		}
		// return -127;
		return std::nullopt;
#endif
	}
	static_assert( *intxp( 1.0f ) == 0 );
	static_assert( *intxp( 10.0f ) == 3 );
	static_assert( *intxp( 1024.0f ) == 10 );
	static_assert( *intxp( 123.45f ) == 6 );

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr Real set_exponent( Real r, std::int32_t exp ) {
		auto uint = DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, r );
		auto result = cxmath_impl::set_exponent_impl<Real>( uint, exp );
		return DAW_BIT_CAST( Real, result );
	}
	static_assert( set_exponent( 2.0f, 2 ) == 4.0f );
	static_assert( set_exponent( 2.0f, 3 ) == 8.0f );
	static_assert( set_exponent( 2.0f, 4 ) == 16.0f );

	template<typename Integer,
	         daw::enable_when_t<std::is_integral_v<Integer>> = nullptr>
	[[nodiscard]] inline constexpr bool is_odd( Integer i ) noexcept {
		return ( static_cast<std::uint32_t>( i ) & 1U ) == 1U;
	}

	template<typename Integer,
	         daw::enable_when_t<std::is_integral_v<Integer>> = nullptr>
	[[nodiscard]] inline constexpr bool is_even( Integer i ) noexcept {
		return ( static_cast<std::uint32_t>( i ) & 1U ) == 0U;
	}

	template<typename Real,
	         daw::enable_when_t<std::is_floating_point_v<Real>> = nullptr>
	[[nodiscard]] constexpr Real abs( Real number ) noexcept {
#if defined( DAW_CX_BIT_CAST )
		auto uint =
		  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, number );
		return DAW_BIT_CAST( Real, cxmath_impl::set_sign_impl<Real>( uint, 0UL ) );
#else
		if( number < (Float)0 ) {
			return -number;
		}
		return number;
#endif
	}

	constexpr void check( float lhs, float rhs ) {
		if( lhs != rhs )
			throw lhs;
	}

	template<typename Real,
	         daw::enable_when_t<std::is_floating_point_v<Real>> = nullptr>
	[[nodiscard]] constexpr float sqrt( Real r ) {
#if defined( DAW_CX_BIT_CAST )
		auto const xi = DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, r );
		switch( cxmath_impl::fp_classify_impl( xi ) ) {
		case fp_classes::zero:
			return 0.0f;
		case fp_classes::nan:
			return std::numeric_limits<float>::quiet_NaN( );
		case fp_classes::infinity:
			return std::numeric_limits<float>::infinity( );
		case fp_classes::subnormal:
		case fp_classes::normal:
			break;
		}

		std::int32_t const N = cxmath_impl::get_exponent_impl<Real>( xi );
		Real const f =
		  DAW_BIT_CAST( Real, cxmath_impl::set_exponent_impl<Real>( xi, 0 ) );
#else
		if( DAW_UNLIKELY( r <= 0.0 ) ) {
			if( r == 0.0 ) {
				return 0.0;
			}
			return daw::numeric_limits<float>::quiet_NaN( );
		}
		auto const exp = intxp( r );
		if( not exp ) {
			return r;
		}
		Real const N = *exp;
		Real const f = cxmath_impl::fexp3( r, 0, N );
#endif
		Real const y0 = ( 0.41731 + ( 0.59016 * f ) );
		Real const z = ( y0 + ( f / y0 ) );
		Real const y2 = ( 0.25 * z ) + ( f / z );
		Real y = 0.5 * ( y2 + ( f / y2 ) );

		if( is_odd( N ) ) {
			y /= sqrt2<Real>;
			return y * fpow2( ( N + 1 ) / 2 );
		}
		return y * fpow2( N / 2 );
	}
	static_assert( sqrt<double>( 16.0 ) == 4.0 );

	template<typename Number, typename Number2,
	         std::enable_if_t<
	           ( (daw::is_integral_v<Number> and daw::is_integral_v<Number2>) or
	             not( std::is_floating_point_v<Number> and
	                  std::is_floating_point_v<Number2> ) )> = nullptr>
	[[nodiscard]] constexpr Number copy_sign( Number x, Number2 s ) noexcept {
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

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr Real copy_sign( Real number, Real sign ) {
		auto const inumber =
		  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, number );
		auto const isign =
		  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, sign );
		auto const new_sign = cxmath_impl::get_sign_raw_impl<Real>( isign );
		auto const result =
		  cxmath_impl::set_sign_raw_impl<Real>( inumber, new_sign );
		return DAW_BIT_CAST( Real, result );
	}

	template<typename Number,
	         daw::enable_when_t<std::is_signed_v<Number>> = nullptr>
	[[nodiscard]] constexpr bool signbit( Number n ) noexcept {
		return n < 0;
	}

	template<typename Number,
	         daw::enable_when_t<not std::is_signed_v<Number>> = nullptr>
	[[nodiscard]] constexpr bool signbit( Number n ) noexcept {
		return false;
	}

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	inline constexpr auto get_significand( Real r ) {
		auto const dint =
		  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, r );
		return cxmath_impl::get_significand_impl<Real>( dint );
	}

	namespace cxmath_impl {
		inline constexpr std::uint64_t get_exponent_raw_impl( std::uint64_t dint ) {
			// Mask off LSB 52 bits and right shift
			// Remove IEEE754 Double bias of 1023
			constexpr std::uint64_t mask = 0x7FF0'0000'0000'0000ULL;
			return ( dint & mask ) >> 52U;
		}
	} // namespace cxmath_impl

	inline constexpr std::uint64_t get_exponent_raw( double d ) {
		auto const dint = DAW_BIT_CAST( std::uint64_t, d );
		return cxmath_impl::get_exponent_raw_impl( dint );
	}

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	inline constexpr std::int32_t get_exponent( Real r ) {
		auto const uint =
		  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, r );
		return cxmath_impl::get_exponent_impl<Real>( uint );
	}
	static_assert( get_exponent( 2.0f ) == 1 );
	static_assert( get_exponent( 4.0f ) == 2 );
	static_assert( get_exponent( 16.0f ) == 4 );

	namespace cxmath_impl {
		inline constexpr double adj_ulp_imp( std::uint64_t i, int adj ) {
			if( adj < 0 ) {
				i -= static_cast<std::uint64_t>( -adj );
			} else {
				i += static_cast<std::uint64_t>( adj );
			}
			return DAW_BIT_CAST( double, i );
		}
	} // namespace cxmath_impl

	inline constexpr double adj_ulp( double d, int adj ) {
		auto i = DAW_BIT_CAST( std::uint64_t, d );
		return cxmath_impl::adj_ulp_imp( i, adj );
	}

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr bool is_inf( Real r ) {
		return fp_classify( r ) == fp_classes::infinity;
	}
	static_assert( not is_inf( daw::numeric_limits<double>::quiet_NaN( ) ) );
	static_assert( not is_inf( daw::numeric_limits<double>::signaling_NaN( ) ) );
	static_assert( is_inf( daw::numeric_limits<double>::infinity( ) ) );
	static_assert( not is_inf( 5.5 ) );
	static_assert( not is_inf( daw::numeric_limits<float>::quiet_NaN( ) ) );
	static_assert( not is_inf( daw::numeric_limits<float>::signaling_NaN( ) ) );
	static_assert( is_inf( daw::numeric_limits<float>::infinity( ) ) );
	static_assert( not is_inf( 5.5f ) );

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr bool is_finite( Real r ) {
		switch( fp_classify( r ) ) {
		case fp_classes::infinity:
		case fp_classes::nan:
			return false;
		case fp_classes::zero:
		case fp_classes::subnormal:
		case fp_classes::normal:
			return true;
		}
	}
	static_assert( not is_finite( daw::numeric_limits<double>::quiet_NaN( ) ) );
	static_assert(
	  not is_finite( daw::numeric_limits<double>::signaling_NaN( ) ) );
	static_assert( not is_finite( daw::numeric_limits<double>::infinity( ) ) );
	static_assert( is_finite( 5.5 ) );

} // namespace daw::cxmath
