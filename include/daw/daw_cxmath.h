// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_arith_traits.h"
#include "daw/daw_assume.h"
#include "daw/daw_attributes.h"
#include "daw/daw_bit_cast.h"
#include "daw/daw_bit_count.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_do_n.h"
#include "daw/daw_enable_if.h"
#include "daw/daw_is_constant_evaluated.h"
#include "daw/daw_likely.h"
#include "daw/daw_uint_buffer.h"
#include "daw/impl/daw_math_impl.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

#if defined( __cpp_lib_bitops )
#if __cpp_lib_bitops >= 201907L and __has_include( <bit> )
#include <bit>
#define DAW_HAS_CPP20_BITOPS
#endif
#endif

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
	} // namespace cxmath_impl

	template<typename Float>
	inline constexpr auto sqrt2 = static_cast<Float>(
	  1.4142135623730950488016887242096980785696718753769480L );

	template<typename Float>
	inline constexpr auto sqrt0_5 = static_cast<Float>(
	  0.7071067811865475244008443621048490392848359376884740L );

	namespace cxmath_impl {
		template<typename Float>
		[[nodiscard]] constexpr Float pow2_impl2( intmax_t exp ) noexcept {
			bool is_neg = exp < 0;
			exp = is_neg ? -exp : exp;
			auto const max_shft =
			  (daw::min)( static_cast<size_t>(
			                daw::numeric_limits<Float>::max_exponent10 ),
			              bit_count_v<std::size_t> );
			Float result = 1.0;

			while( static_cast<std::size_t>( exp ) >= max_shft ) {
				result *= static_cast<Float>( max_value<std::size_t> );
				exp -= max_shft;
			}
			if( exp > 0 ) {
				result *= static_cast<Float>( 1ULL << static_cast<size_t>( exp ) );
			}
			if( is_neg and result != 0.0 ) {
				result = static_cast<Float>( 1.0 ) / result;
			}
			return result;
		}

		[[nodiscard]] constexpr size_t pow10_impl( intmax_t exp ) noexcept {
			// exp is < floor( log10( max_value<std::size_t> ) )
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
			  (daw::min)( daw::numeric_limits<Float>::max_exponent10,
			              daw::numeric_limits<size_t>::digits10 );
			Float result = 1.0;

			while( exp >= max_spow ) {
				result *= static_cast<Float>( pow10_impl<max_spow>( ) );
				exp -= max_spow;
			}
			if( exp > 0 ) {
				result *= static_cast<Float>( pow10_impl( exp ) );
			}
			if( is_neg and result != 0.0 ) {
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
	} // namespace cxmath_impl

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

	// Adapted from Hackers delight 11-3.  Do a binary decomposition of n
	template<typename Arithmetic, typename Unsigned>
	constexpr Arithmetic pow_n( Arithmetic x, Unsigned n ) {
		static_assert( daw::is_arithmetic_v<Arithmetic> );
		static_assert( daw::is_unsigned_v<Unsigned> );
		auto result = Arithmetic{ 1 };
		auto p = x;
		while( true ) {
			if( n & 1 ) {
				result = p * result;
			}
			n >>= 1;
			if( n == 0 ) {
				return result;
			}
			p *= p;
		}
	}

#if __has_builtin( __builtin_clzg )
	template<typename Unsigned, std::enable_if_t<daw::is_unsigned_v<Unsigned>, std::nullptr_t> = nullptr>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint32_t count_leading_zeroes( Unsigned u ) {
		return __builtin_clzg( u );
	}
#else

#if DAW_HAS_BUILTIN( __builtin_clz )
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr unsigned
	count_leading_zeroes( unsigned v ) noexcept {
		if( v != 0U ) {
			return static_cast<unsigned>( __builtin_clz( v ) );
		}
		return static_cast<unsigned>( bit_count_v<unsigned> );
	}

	[[nodiscard]] DAW_ATTRIB_INLINE constexpr unsigned
	count_leading_zeroes( unsigned long v ) noexcept {
		if( v != 0U ) {
			return static_cast<unsigned>( __builtin_clzl( v ) );
		}
		return static_cast<unsigned>( bit_count_v<unsigned long> );
	}


#if DAW_HAS_BUILTIN( __builtin_clzll )
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr unsigned
	count_leading_zeroes( unsigned long long v ) noexcept {
		if( v != 0U ) {
			return static_cast<unsigned>( __builtin_clzll( v ) );
		}
		return static_cast<unsigned>( bit_count_v<unsigned long long> );
	}
#else
	[[nodiscard]] DAW_ATTRIB_INLINE inline constexpr unsigned
	count_leading_zeroes( std::uint64_t v ) noexcept {
		if( v == 0 ) {
			return 64U;
		}
		auto high = static_cast<std::uint32_t>( v >> 32U );
		if( high != 0 ) {
			return static_cast<std::uint32_t>( __builtin_clz( high ) );
		}
		auto low = static_cast<std::uint32_t>( v );
		if( low != 0 ) {
			return 32U + static_cast<std::uint32_t>( __builtin_clz( low ) );
		}
		return 64U;
	}

#endif

#else
	// Based on code from
	// https://graphics.stanford.edu/~seander/bithacks.html
	[[nodiscard]] constexpr std::uint32_t
	count_leading_zeroes( std::uint64_t v ) noexcept {
		if( v == 0 ) {
			return 64U;
		}
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

	[[nodiscard]] DAW_ATTRIB_FLATTEN constexpr std::uint32_t
	count_leading_zeros( daw::uint128_t v ) {
		if( v == 0 ) {
			return 128U;
		}
		auto const h =
		  count_leading_zeros( static_cast<std::uint64_t>( v >> 64ULL ) );
		if( h == 64U ) {
			return count_leading_zeros( static_cast<std::uint64_t>( v ) );
		}
		return h;
	}
#endif
		[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr unsigned
		count_leading_zeroes( daw::UInt32 v ) noexcept {
			return count_leading_zeroes( static_cast<std::uint32_t>( v ) );
		}

		[[nodiscard]] DAW_ATTRIB_FLATINLINE inline constexpr unsigned
		count_leading_zeroes( daw::UInt64 v ) noexcept {
			return count_leading_zeroes( static_cast<std::uint64_t>( v ) );
		}

	namespace cxmath_impl {
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint32_t
		count_trailing_zeros_cx32( std::uint32_t v ) noexcept {
			if( v == 0 ) {
				return 32U;
			}
			std::uint32_t c = 32U;
			v &= static_cast<std::uint32_t>( -static_cast<std::int32_t>( v ) );
			if( v ) {
				c--;
			}
			if( v & 0x0000'FFFFU ) {
				c -= 16;
			}
			if( v & 0x00FF'00FFU ) {
				c -= 8;
			}
			if( v & 0x0F0F'0F0FU ) {
				c -= 4;
			}
			if( v & 0x3333'3333U ) {
				c -= 2;
			}
			if( v & 0x5555'5555U ) {
				c -= 1;
			}
			return c;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint32_t
		count_trailing_zeros_cx64( std::uint64_t v ) noexcept {
			if( v == 0 ) {
				return 64U;
			}
			if( ( v & 0xFFFF'FFFFU ) == 0 ) {
				return 32 + count_trailing_zeros_cx32(
				              static_cast<std::uint32_t>( v >> 32U ) );
			}
			return count_trailing_zeros_cx32( static_cast<std::uint32_t>( v ) );
		}

		template<typename T>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint32_t
		count_trailing_zeros_cx( T v ) noexcept {
			static_assert( sizeof( T ) == 8 or sizeof( T ) == 4 );
			if constexpr( sizeof( T ) == 8 ) {
				return count_trailing_zeros_cx64( v );
			} else {
				return count_trailing_zeros_cx32( v );
			}
		}
	} // namespace cxmath_impl

#if defined( DAW_HAS_CPP20_BITOPS )
	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint32_t v ) noexcept {
		return static_cast<std::uint32_t>( std::countr_zero( v ) );
	}
	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint64_t v ) noexcept {
		return static_cast<std::uint32_t>( std::countr_zero( v ) );
	}
#elif DAW_HAS_BUILTIN( __builtin_ctz ) or defined( __GNUC__ ) or \
  defined( __bultin_ctz )

	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint32_t v ) noexcept {
		if( v == 0 ) {
			return 32U;
		}
#if INT_MAX == 2147483647LL
		return static_cast<std::uint32_t>(
		  __builtin_ctz( static_cast<unsigned>( v ) ) );
#elif LONG_MAX == 2147483647LL
		return static_cast<std::uint32_t>(
		  __builtin_ctzl( static_cast<unsigned long>( v ) ) );
#elif LLONG_MAX == 2147483647LL
		return static_cast<std::uint32_t>(
		  __builtin_ctzll( static_cast<unsigned long long>( v ) ) );
#else
#error Unsupported int sizes
#endif
	}

	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint64_t v ) noexcept {
		if( v == 0 ) {
			return 64U;
		}
#if INT_MAX == 9223372036854775807LL
		return static_cast<std::uint32_t>(
		  __builtin_ctz( static_cast<unsigned>( v ) ) );
#elif LONG_MAX == 9223372036854775807LL
		return static_cast<std::uint32_t>(
		  __builtin_ctzl( static_cast<unsigned long>( v ) ) );
#elif LLONG_MAX == 9223372036854775807LL
		return static_cast<std::uint32_t>(
		  __builtin_ctzll( static_cast<unsigned long long>( v ) ) );
#else
#error Unsupported int sizes
#endif
	}
#elif defined( _MSC_VER ) and defined( _M_X64 ) and \
  defined( DAW_IS_CONSTANT_EVALUATED )
	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint32_t v ) noexcept {
#if defined( DAW_IS_CONSTANT_EVALUATED )
		if( DAW_IS_CONSTANT_EVALUATED( ) ) {
			return cxmath_impl::count_trailing_zeros_cx( v );
		} else {
			return static_cast<std::uint32_t>( _tzcnt_u32( v ) );
		}
#else
		return cxmath_impl::count_trailing_zeros_cx( v );
#endif
	}

	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint64_t v ) noexcept {
#if defined( DAW_IS_CONSTANT_EVALUATED )
		if( DAW_IS_CONSTANT_EVALUATED( ) ) {
			return cxmath_impl::count_trailing_zeros_cx( v );
		} else {
			return static_cast<std::uint32_t>( _tzcnt_u64( v ) );
		}
#else
		return cxmath_impl::count_trailing_zeros_cx( v );
#endif
	}
#else
	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint32_t v ) noexcept {
		return cxmath_impl::count_trailing_zeros_cx( v );
	}
	[[nodiscard]] constexpr std::uint32_t
	count_trailing_zeros( std::uint64_t v ) noexcept {
		return cxmath_impl::count_trailing_zeros_cx( v );
	}
#endif

	template<typename Number, std::enable_if_t<daw::is_arithmetic_v<Number>,
	                                           std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr Number pow( Number b, int32_t exp ) noexcept {
		auto result = Number{ 1 };
		while( exp < Number{ 0 } ) {
			result /= b;
			++exp;
		}
		while( exp > Number{ 0 } ) {
			result *= b;
			--exp;
		}
		return result;
	}

	template<typename Real, std::enable_if_t<
#ifdef DAW_CX_BIT_CAST
	                          std::is_floating_point_v<Real>,
#else
	                          std::is_same_v<Real, float>,
#endif
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr std::optional<std::int32_t> intxp( Real f ) noexcept {
#if defined( DAW_CX_BIT_CAST )
		auto const uint =
		  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, f );
		return cxmath_impl::get_exponent_impl<Real>( uint );
#else
		if( f == 0.0f ) {
			return static_cast<std::int32_t>( 0 );
		}
		if( f > max_value<Real> ) {
			// inf
			return std::nullopt;
		}
		if( f < -max_value<Real> ) {
			// -inf
			return std::nullopt;
		}
		if( f != f ) {
			// NaN
			return std::nullopt;
		}
		int32_t exponent = 254;
		Real abs_f = f < 0 ? -f : f;

		while( abs_f < 0x1p87f ) {
			abs_f *= 0x1p41f;
			exponent -= 41;
		}

		auto const a = static_cast<std::uint64_t>( abs_f * 0x1p-64f );
		auto lz = static_cast<int32_t>( count_leading_zeroes( a ) );
		exponent -= lz;

		if( exponent >= 0 ) {
			return static_cast<std::int32_t>( exponent - 127 );
		}
		return std::nullopt;
#endif
	}
	static_assert( *intxp( 1.0f ) == 0 );
	static_assert( *intxp( 10.0f ) == 3 );
	static_assert( *intxp( 1024.0f ) == 10 );
	static_assert( *intxp( 123.45f ) == 6 );

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	constexpr Real pow2( std::int32_t exp ) noexcept {
#if defined( DAW_CX_BIT_CAST )
		if( exp >= daw::numeric_limits<Real>::max_exponent ) {
			return daw::numeric_limits<Real>::infinity( );
		} else if( exp <= daw::numeric_limits<Real>::min_exponent ) {
			return Real{ 0 };
		}
		auto result = cxmath_impl::set_exponent_impl<Real>(
		  cxmath_impl::unsigned_float_type_t<Real>{ 0 }, exp );
		return DAW_BIT_CAST( Real, result );
#else
		return cxmath_impl::pow2_t<double>::get<Real>( exp );
#endif
	}

	inline constexpr float fpow2( std::int32_t exp ) noexcept {
		return pow2<float>( exp );
	}
	static_assert( fpow2( 1 ) == 2.0f );
	static_assert( fpow2( 2 ) == 4.0f );

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr Real set_exponent( Real r,
	                                           std::int32_t exponent ) noexcept {
#if defined( DAW_CX_BIT_CAST )
		using UInt = cxmath_impl::unsigned_float_type_t<Real>;
		auto u = DAW_BIT_CAST( UInt, r );
		UInt result = cxmath_impl::set_exponent_impl<Real>( u, exponent );
		return DAW_BIT_CAST( Real, result );
#else
		std::int32_t const exp_diff = exponent - *intxp( r );
		if( exp_diff > 0 ) {
			return pow2<Real>( exp_diff ) * r;
		}
		return r / pow2<Real>( -exp_diff );
#endif
	}
	static_assert( set_exponent( 2.0f, 1 ) == 2.0f );
	static_assert( set_exponent( 2.0f, 2 ) == 4.0f );
	static_assert( set_exponent( 2.0f, 4 ) == 16.0f );
#if defined( DAW_CX_BIT_CAST )
	static_assert( set_exponent( 2.0, 1 ) == 2.0 );
	static_assert( set_exponent( 2.0, 2 ) == 4.0 );
	static_assert( set_exponent( 2.0, 4 ) == 16.0 );
#endif

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
				return static_cast<std::int16_t>(
				  static_cast<std::int16_t>( raw_exponent( ) ) - bias );
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
					return result * set_exponent( 2.0f, -e );
				}
				return result / set_exponent( 2.0f, -e );
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
			} else if( f > max_value<float> ) {
				// infinity
				return { 0x7f80'0000, f };
			} else if( f < lowest_value<float> ) {
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

			auto significand =
			  static_cast<std::uint32_t>( ( a << ( lz + 1 ) ) >> ( 64 - 23 ) ); // [3]
			return { ( static_cast<std::uint32_t>( sign ? 1U : 0U ) << 31U ) |
			           ( static_cast<std::uint32_t>( exponent ) << 23U ) |
			           significand,
			         f };
#endif
		}

		template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
		                                         std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr Real fexp3( Real f, std::int32_t exponent,
		                                    std::int32_t old_exponent ) noexcept {
			std::int32_t const exp_diff = exponent - old_exponent;
			if( exp_diff > 0 ) {
				return set_exponent( Real{ 2.0 }, exp_diff ) * f;
			}
			return f / set_exponent( Real{ 2.0 }, -exp_diff );
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
#if defined( DAW_CX_BIT_CAST )
		return fp_classify( r ) == fp_classes::nan;
#else
		return ( r != r );
#endif
	}
	static_assert( is_nan( daw::numeric_limits<double>::quiet_NaN( ) ) );
	static_assert( is_nan( daw::numeric_limits<double>::signaling_NaN( ) ) );
	static_assert( not is_nan( daw::numeric_limits<double>::infinity( ) ) );
	static_assert( is_nan( daw::numeric_limits<float>::quiet_NaN( ) ) );
	static_assert( is_nan( daw::numeric_limits<float>::signaling_NaN( ) ) );
	static_assert( not is_nan( daw::numeric_limits<float>::infinity( ) ) );

#if defined( DAW_CX_BIT_CAST )
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
#endif

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

	template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
	                                            std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr bool is_odd( Integer i ) noexcept {
		return ( std::make_unsigned_t<daw::remove_cvref_t<Integer>>( i ) & 1U ) ==
		       1U;
	}

	template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
	                                            std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr bool is_even( Integer i ) noexcept {
		return ( std::make_unsigned_t<daw::remove_cvref_t<Integer>>( i ) & 1U ) ==
		       1U;
	}

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr Real abs( Real number ) noexcept {
		if( DAW_IS_CONSTANT_EVALUATED_COMPAT( ) and not daw_has_cx_cmath ) {
#if defined( DAW_CX_BIT_CAST )
			auto uint =
			  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, number );
			using uint_t = decltype( uint );
			return DAW_BIT_CAST(
			  Real, cxmath_impl::set_sign_impl<Real>( uint, uint_t{ 0 } ) );
#else
			if( number < Real{ 0 } ) {
				return -number;
			}
			return number;
#endif
		} else {
			return std::abs( number );
		}
	}

#if defined( DAW_CX_BIT_CAST )
	namespace cxmath_impl {

		template<
		  typename Real, typename UInt,
		  std::enable_if_t<is_matching_v<Real, UInt>, std::nullptr_t> = nullptr>
		constexpr Real adjust_exponent_impl( UInt u, std::int32_t exponent ) {
			auto current_expoent = get_exponent_impl<Real>( u );
			current_expoent += exponent;
			return set_exponent_impl<Real>( u, current_expoent );
		}
	} // namespace cxmath_impl
	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	constexpr Real adjust_exponent( Real r, std::int32_t exponent ) {
		auto result = cxmath_impl::adjust_exponent_impl(
		  DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, r ), exponent );
		return DAW_BIT_CAST( Real, result );
	}
#endif

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr Real sqrt_fast( Real r ) {
		static_assert( not std::is_same_v<Real, long double>,
		               "Long double not supported" );
#if false and defined( DAW_CX_BIT_CAST )
		auto const xi = DAW_BIT_CAST( cxmath_impl::unsigned_float_type_t<Real>, r );

		std::int32_t const N = cxmath_impl::get_exponent_impl<Real>( xi );
		Real const f =
		  DAW_BIT_CAST( Real, cxmath_impl::set_exponent_impl<Real>( xi, 0 ) );
#else
		auto const exp = intxp( r );
		if( not exp ) {
			return r;
		}
		std::int32_t const N = *exp;
		Real const f = static_cast<Real>( cxmath_impl::fexp3( r, 0, N ) );
#endif
		Real y0 =
		  ( static_cast<Real>( 0.41731 ) + ( static_cast<Real>( 0.59016 ) * f ) );
		// Round 1
		y0 = static_cast<Real>( 0.5 ) * ( y0 + ( f / y0 ) );
		if constexpr( not std::is_same_v<Real, float> ) {
			y0 = static_cast<Real>( 0.5 ) * ( y0 + ( f / y0 ) );
		}

		// Final Round
		Real const z = ( y0 + ( f / y0 ) );
		Real const y2 = ( Real{ 0.25 } * z ) + ( f / z );
		Real y = Real{ 0.5 } * ( y2 + ( f / y2 ) );
		if( is_odd( N ) ) {
			y /= sqrt2<Real>;
			return y * pow2<Real>( ( N + 1 ) / 2 );
		}
		return y * pow2<Real>( N / 2 );
	}

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr Real sqrt( Real r ) {
#if defined( DAW_CX_BIT_CAST )
		switch( fp_classify( r ) ) {
		case fp_classes::zero:
			return Real{ 0 };
		case fp_classes::nan:
			return std::numeric_limits<Real>::quiet_NaN( );
		case fp_classes::infinity:
			return std::numeric_limits<Real>::infinity( );
		case fp_classes::subnormal:
		case fp_classes::normal:
		default:
			break;
		}
		return sqrt_fast( r );
#else
		if( DAW_UNLIKELY( r <= Real{ 0 } ) ) {
			if( r == Real{ 0 } ) {
				return Real{ 0 };
			}
			return daw::numeric_limits<Real>::quiet_NaN( );
		}
		return sqrt_fast( r );
#endif
	}
#if defined( DAW_CX_BIT_CAST )
	static_assert( sqrt<double>( 16.0 ) == 4.0 );
#endif

	template<typename Number, typename Number2
#if defined( DAW_CX_BIT_CAST )
	         ,
	         std::enable_if_t<
	           ( (daw::is_integral_v<Number> and daw::is_integral_v<Number2>) or
	             not( std::is_floating_point_v<Number> and
	                  std::is_floating_point_v<Number2> ) )> = nullptr
#endif
	         >
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

#if defined( DAW_CX_BIT_CAST )
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
#endif

	template<typename Number,
	         daw::enable_when_t<std::is_signed_v<Number>> = nullptr>
	[[nodiscard]] constexpr bool signbit( Number n ) noexcept {
		return n < 0;
	}

	template<typename Number,
	         daw::enable_when_t<not std::is_signed_v<Number>> = nullptr>
	[[nodiscard]] constexpr bool signbit( Number ) noexcept {
		return false;
	}

#if defined( DAW_CX_BIT_CAST )
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
#endif

	template<typename Real, std::enable_if_t<std::is_floating_point_v<Real>,
	                                         std::nullptr_t> = nullptr>
	[[nodiscard]] inline constexpr bool is_inf( Real r ) {
#if defined( DAW_CX_BIT_CAST )
		return fp_classify( r ) == fp_classes::infinity;
#else
		return ( r == daw::numeric_limits<Real>::infinity( ) ) |
		       ( r == -daw::numeric_limits<Real>::infinity( ) );
#endif
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
#if defined( DAW_CX_BIT_CAST )
		switch( fp_classify( r ) ) {
		case fp_classes::infinity:
		case fp_classes::nan:
			return false;
		case fp_classes::zero:
		case fp_classes::subnormal:
		case fp_classes::normal:
		default:
			return true;
		}
#else
		return not is_nan( r ) and not is_inf( r );
#endif
	}
	static_assert( not is_finite( daw::numeric_limits<double>::quiet_NaN( ) ) );
	static_assert(
	  not is_finite( daw::numeric_limits<double>::signaling_NaN( ) ) );
	static_assert( not is_finite( daw::numeric_limits<double>::infinity( ) ) );
	static_assert( is_finite( 5.5 ) );

	template<typename Unsigned>
	Unsigned round_up_pow2( Unsigned v ) {
		if( v == Unsigned{ 0 } ) {
			return 0;
		}
		std::uint32_t lz = daw::cxmath::count_leading_zeroes( v );
		Unsigned const msb = ( ( bit_count_v<Unsigned> - Unsigned{ 1 } ) - lz );
		Unsigned res = Unsigned{ 1 } << msb;
		if( res < v ) {
			res <<= 1U;
		}
		return res;
	}

#if false and defined( DAW_HAS_CPP20_BITOPS )
	template<typename Integer>
	DAW_ATTRIB_INLINE auto constexpr popcount( Integer i ) {
		return std::popcount( i );
	}
#elif DAW_HAS_BUILTIN( __builtin_popcount ) and \
  DAW_HAS_BUILTIN( __builtin_popcountl ) and    \
  DAW_HAS_BUILTIN( __builtin_popcountll )
	template<typename Integer>
	DAW_ATTRIB_INLINE auto constexpr popcount( Integer i ) {
		static_assert( std::is_integral_v<Integer>,
		               "Only integer types are allowed" );
		if constexpr( sizeof( Integer ) <= sizeof( int ) ) {
			return __builtin_popcount( static_cast<unsigned>( i ) );
		} else if constexpr( sizeof( Integer ) == sizeof( long ) ) {
			return __builtin_popcountl( static_cast<unsigned long>( i ) );
#if defined( DAW_HAS_INT128 )
		} else if constexpr( sizeof( Integer ) == sizeof( daw::int128_t ) ) {
			auto const lo =
			  static_cast<std::uint64_t>( static_cast<daw::uint128_t>( i ) );
			auto const hi =
			  static_cast<std::uint64_t>( static_cast<daw::uint128_t>( i ) >> 64 );
			return __builtin_popcountll( lo ) + __builtin_popcountll( hi );
#endif
		} else {
			static_assert( sizeof( Integer ) == sizeof( long long ),
			               "Unexpected integer size" );
			return __builtin_popcountll( static_cast<unsigned long long>( i ) );
		}
	}
#else
	template<typename Integer>
	DAW_ATTRIB_INLINE int constexpr popcount( Integer i ) {
		auto ui = [i] {
			if constexpr( sizeof( Integer ) <= sizeof( int ) ) {
				return static_cast<unsigned>( i );
			} else if constexpr( sizeof( Integer ) == sizeof( long ) ) {
				return static_cast<unsigned long>( i );
#if defined( DAW_HAS_INT128 )
			} else if constexpr( sizeof( Integer ) == sizeof( daw::int128_t ) ) {
				return static_cast<daw::uint128_t>( i );
#endif
			} else {
				static_assert( sizeof( Integer ) == sizeof( long long ),
				               "Unexpected integer size" );
				return static_cast<unsigned long long>( i );
			}
		}( );
		int count = 0;
		while( ui != 0 ) {
			count += ui & 1;
			ui >>= 1;
		}
		return count;
	}
#endif

	template<
	  typename..., typename Integer,
	  std::enable_if_t<std::is_integral_v<Integer>, std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr auto to_unsigned( Integer value ) {
		return static_cast<daw::make_unsigned_t<Integer>>( value );
	}

	template<
	  typename..., typename Integer,
	  std::enable_if_t<std::is_integral_v<Integer>, std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr auto to_signed( Integer value ) {
		return static_cast<daw::make_signed_t<Integer>>( value );
	}

	inline constexpr std::uint64_t powers_of_ten[19] = {
	  10ull,
	  100ull,
	  1'000ull,
	  10'000ull,
	  100'000ull,
	  1'000'000ull,
	  10'000'000ull,
	  100'000'000ull,
	  1'000'000'000ull,
	  10'000'000'000ull,
	  100'000'000'000ull,
	  1'000'000'000'000ull,
	  10'000'000'000'000ull,
	  100'000'000'000'000ull,
	  1'000'000'000'000'000ull,
	  10'000'000'000'000'000ull,
	  100'000'000'000'000'000ull,
	  1'000'000'000'000'000'000ull,
	  10'000'000'000'000'000'000ull };

	constexpr int count_digits( std::uint64_t value ) {
		auto b = -( value > 0 ) & ( 63 - count_leading_zeroes( value ) );
		auto a = ( b * 77 ) / 256;
		return static_cast<int>( 1 + a + ( value >= powers_of_ten[a] ) );
	}
	static_assert( count_digits( 1'000'000ULL ) == 7 );
} // namespace daw::cxmath
