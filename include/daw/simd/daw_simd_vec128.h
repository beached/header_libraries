// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

// A 16 byte vector of 8, 16 or 32 bit unsigned lanes with the integer
// arithmetic the byte vectors do not have, including widening multiply-adds.
// It is 16 bytes on every backend so that short runs, e.g. the 8 digits of a
// number, never need a 32 or 64 byte load.  Every backend gives the same
// results; where an instruction differs, e.g. signed weights or saturation,
// the precondition on the operation keeps the results equal.
// Included by daw_simd.h after a backend is chosen.

#include <daw/daw_attributes.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace daw::simd_impl {
	template<typename T>
	class vec128 {
		static_assert( std::is_same_v<T, std::uint8_t> or
		                 std::is_same_v<T, std::uint16_t> or
		                 std::is_same_v<T, std::uint32_t>,
		               "vec128 lanes are 8, 16 or 32 bit unsigned integers" );

#if defined( DAW_HAS_NEON_SIMD )
		using native_t = uint8x16_t;
#else
		using native_t = __m128i;
#endif
		native_t m_value;

		// Lanes twice as wide, for mul_add_pairs
		using widened_t =
		  std::conditional_t<sizeof( T ) == 1, std::uint16_t, std::uint32_t>;

		template<typename>
		friend class vec128;

		DAW_ATTRIB_INLINE explicit vec128( native_t value ) noexcept
		  : m_value( value ) {}

#if defined( DAW_HAS_NEON_SIMD )
		[[nodiscard]] DAW_ATTRIB_INLINE uint16x8_t u16( ) const noexcept {
			return vreinterpretq_u16_u8( m_value );
		}
		[[nodiscard]] DAW_ATTRIB_INLINE uint32x4_t u32( ) const noexcept {
			return vreinterpretq_u32_u8( m_value );
		}
#endif

		/// even in the even lanes and odd in the odd lanes as one lane of twice
		/// the width
		[[nodiscard]] static constexpr std::uint64_t pair( T even, T odd ) {
			return static_cast<std::uint64_t>( even ) |
			       ( static_cast<std::uint64_t>( odd ) << ( 8U * sizeof( T ) ) );
		}

	public:
		using value_type = T;

		[[nodiscard]] static constexpr std::size_t size( ) noexcept {
			return 16U / sizeof( T );
		}

		/// Every lane is value
		DAW_ATTRIB_INLINE explicit vec128( T value ) noexcept
#if defined( DAW_HAS_NEON_SIMD )
		  : m_value( [&] {
			  if constexpr( sizeof( T ) == 1 ) {
				  return vdupq_n_u8( value );
			  } else if constexpr( sizeof( T ) == 2 ) {
				  return vreinterpretq_u8_u16( vdupq_n_u16( value ) );
			  } else {
				  return vreinterpretq_u8_u32( vdupq_n_u32( value ) );
			  }
		  }( ) ) {
		}
#else
		  : m_value( [&] {
			  if constexpr( sizeof( T ) == 1 ) {
				  return _mm_set1_epi8( static_cast<char>( value ) );
			  } else if constexpr( sizeof( T ) == 2 ) {
				  return _mm_set1_epi16( static_cast<short>( value ) );
			  } else {
				  return _mm_set1_epi32( static_cast<int>( value ) );
			  }
		  }( ) ) {
		}
#endif

		/// even in the even lanes and odd in the odd lanes
		[[nodiscard]] DAW_ATTRIB_INLINE static vec128 pattern( T even, T odd ) noexcept {
			auto const lanes = pair( even, odd );
#if defined( DAW_HAS_NEON_SIMD )
			if constexpr( sizeof( T ) == 4 ) {
				return vec128( vreinterpretq_u8_u64( vdupq_n_u64( lanes ) ) );
			} else if constexpr( sizeof( T ) == 2 ) {
				return vec128( vreinterpretq_u8_u32(
				  vdupq_n_u32( static_cast<std::uint32_t>( lanes ) ) ) );
			} else {
				return vec128( vreinterpretq_u8_u16(
				  vdupq_n_u16( static_cast<std::uint16_t>( lanes ) ) ) );
			}
#else
			if constexpr( sizeof( T ) == 4 ) {
				return vec128( _mm_set1_epi64x( static_cast<long long>( lanes ) ) );
			} else if constexpr( sizeof( T ) == 2 ) {
				return vec128( _mm_set1_epi32( static_cast<int>( lanes ) ) );
			} else {
				return vec128( _mm_set1_epi16( static_cast<short>( lanes ) ) );
			}
#endif
		}

		/// The 16 bytes at ptr
		[[nodiscard]] DAW_ATTRIB_INLINE static vec128
		load( void const *ptr ) noexcept {
#if defined( DAW_HAS_NEON_SIMD )
			return vec128( vld1q_u8( static_cast<std::uint8_t const *>( ptr ) ) );
#else
			return vec128( _mm_loadu_si128( static_cast<__m128i const *>( ptr ) ) );
#endif
		}

		/// The 8 bytes at ptr in the low half, the high half is 0
		[[nodiscard]] DAW_ATTRIB_INLINE static vec128
		load_low( void const *ptr ) noexcept {
#if defined( DAW_HAS_NEON_SIMD )
			return vec128(
			  vcombine_u8( vld1_u8( static_cast<std::uint8_t const *>( ptr ) ),
			               vdup_n_u8( 0 ) ) );
#else
			return vec128( _mm_loadl_epi64( static_cast<__m128i const *>( ptr ) ) );
#endif
		}

		/// Lane I
		template<std::size_t I>
		[[nodiscard]] DAW_ATTRIB_INLINE T get( ) const noexcept {
			static_assert( I < size( ) );
#if defined( DAW_HAS_NEON_SIMD )
			if constexpr( sizeof( T ) == 1 ) {
				return vgetq_lane_u8( m_value, I );
			} else if constexpr( sizeof( T ) == 2 ) {
				return vgetq_lane_u16( u16( ), I );
			} else {
				return vgetq_lane_u32( u32( ), I );
			}
#else
			if constexpr( sizeof( T ) == 4 and I == 0 ) {
				return static_cast<T>( _mm_cvtsi128_si32( m_value ) );
			} else if constexpr( sizeof( T ) == 4 ) {
				return static_cast<T>( _mm_extract_epi32( m_value, I ) );
			} else if constexpr( sizeof( T ) == 2 ) {
				return static_cast<T>( _mm_extract_epi16( m_value, I ) );
			} else {
				return static_cast<T>( _mm_extract_epi8( m_value, I ) );
			}
#endif
		}

		/// Lanewise, wrapping
		[[nodiscard]] DAW_ATTRIB_INLINE friend vec128 operator+( vec128 lhs,
		                                                         vec128 rhs ) noexcept {
#if defined( DAW_HAS_NEON_SIMD )
			if constexpr( sizeof( T ) == 1 ) {
				return vec128( vaddq_u8( lhs.m_value, rhs.m_value ) );
			} else if constexpr( sizeof( T ) == 2 ) {
				return vec128( vreinterpretq_u8_u16( vaddq_u16( lhs.u16( ), rhs.u16( ) ) ) );
			} else {
				return vec128( vreinterpretq_u8_u32( vaddq_u32( lhs.u32( ), rhs.u32( ) ) ) );
			}
#else
			if constexpr( sizeof( T ) == 1 ) {
				return vec128( _mm_add_epi8( lhs.m_value, rhs.m_value ) );
			} else if constexpr( sizeof( T ) == 2 ) {
				return vec128( _mm_add_epi16( lhs.m_value, rhs.m_value ) );
			} else {
				return vec128( _mm_add_epi32( lhs.m_value, rhs.m_value ) );
			}
#endif
		}

		/// Lanewise, wrapping
		[[nodiscard]] DAW_ATTRIB_INLINE friend vec128 operator-( vec128 lhs,
		                                                         vec128 rhs ) noexcept {
#if defined( DAW_HAS_NEON_SIMD )
			if constexpr( sizeof( T ) == 1 ) {
				return vec128( vsubq_u8( lhs.m_value, rhs.m_value ) );
			} else if constexpr( sizeof( T ) == 2 ) {
				return vec128( vreinterpretq_u8_u16( vsubq_u16( lhs.u16( ), rhs.u16( ) ) ) );
			} else {
				return vec128( vreinterpretq_u8_u32( vsubq_u32( lhs.u32( ), rhs.u32( ) ) ) );
			}
#else
			if constexpr( sizeof( T ) == 1 ) {
				return vec128( _mm_sub_epi8( lhs.m_value, rhs.m_value ) );
			} else if constexpr( sizeof( T ) == 2 ) {
				return vec128( _mm_sub_epi16( lhs.m_value, rhs.m_value ) );
			} else {
				return vec128( _mm_sub_epi32( lhs.m_value, rhs.m_value ) );
			}
#endif
		}

		/// Lane n of the result, which has lanes twice as wide, is
		/// v[2n] * w[2n] + v[2n + 1] * w[2n + 1].  For 8 bit lanes every w lane
		/// must be < 128 and every result <= 32767.  For 16 bit lanes every v
		/// and w lane must be < 32768
		[[nodiscard]] DAW_ATTRIB_INLINE friend vec128<widened_t>
		mul_add_pairs( vec128 v, vec128 w ) noexcept {
			static_assert( sizeof( T ) < 4, "32 bit lanes cannot be widened" );
#if defined( DAW_HAS_NEON_SIMD )
			if constexpr( sizeof( T ) == 1 ) {
				auto const lo =
				  vmull_u8( vget_low_u8( v.m_value ), vget_low_u8( w.m_value ) );
				auto const hi =
				  vmull_u8( vget_high_u8( v.m_value ), vget_high_u8( w.m_value ) );
#if defined( __aarch64__ )
				return vec128<widened_t>( vreinterpretq_u8_u16( vpaddq_u16( lo, hi ) ) );
#else
				return vec128<widened_t>( vreinterpretq_u8_u16(
				  vcombine_u16( vpadd_u16( vget_low_u16( lo ), vget_high_u16( lo ) ),
				                vpadd_u16( vget_low_u16( hi ), vget_high_u16( hi ) ) ) ) );
#endif
			} else {
				auto const lo =
				  vmull_u16( vget_low_u16( v.u16( ) ), vget_low_u16( w.u16( ) ) );
				auto const hi =
				  vmull_u16( vget_high_u16( v.u16( ) ), vget_high_u16( w.u16( ) ) );
#if defined( __aarch64__ )
				return vec128<widened_t>( vreinterpretq_u8_u32( vpaddq_u32( lo, hi ) ) );
#else
				return vec128<widened_t>( vreinterpretq_u8_u32(
				  vcombine_u32( vpadd_u32( vget_low_u32( lo ), vget_high_u32( lo ) ),
				                vpadd_u32( vget_low_u32( hi ), vget_high_u32( hi ) ) ) ) );
#endif
			}
#else
			if constexpr( sizeof( T ) == 1 ) {
				// The weights are signed and the sums saturate, equal under the
				// precondition
				return vec128<widened_t>( _mm_maddubs_epi16( v.m_value, w.m_value ) );
			} else {
				// Signed 16 bit lanes, equal under the precondition
				return vec128<widened_t>( _mm_madd_epi16( v.m_value, w.m_value ) );
			}
#endif
		}

		/// For 32 bit lanes: lanes 0 to 3 of the result are v's lanes limited to
		/// 65535, lanes 4 to 7 repeat them.  Every v lane must be < 2^31
		[[nodiscard]] DAW_ATTRIB_INLINE friend vec128<std::uint16_t>
		pack( vec128 v ) noexcept {
			static_assert( sizeof( T ) == 4, "pack narrows 32 bit lanes" );
#if defined( DAW_HAS_NEON_SIMD )
			auto const narrow = vqmovn_u32( v.u32( ) );
			return vec128<std::uint16_t>(
			  vreinterpretq_u8_u16( vcombine_u16( narrow, narrow ) ) );
#else
			return vec128<std::uint16_t>( _mm_packus_epi32( v.m_value, v.m_value ) );
#endif
		}
	};
} // namespace daw::simd_impl
