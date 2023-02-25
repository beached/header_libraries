// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if defined( _MSC_VER ) and not defined( __clang__ )

#include "daw/daw_arith_traits.h"
#include "daw/daw_attributes.h"
#include "daw/daw_consteval.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_is_constant_evaluated.h"
#include "daw/daw_likely.h"
#include "daw_signed_error_handling.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <exception>
#include <intrin.h>
#include <limits>
#include <type_traits>
#include <utility>

namespace daw::integers::sint_impl {
	template<typename T>
	inline constexpr bool is_valid_int_type =
	  std::is_integral_v<T> and std::is_signed_v<T> and
	  sizeof( T ) <= sizeof( std::int64_t );

	template<typename SignedInteger>
	constexpr bool wrapping_add( SignedInteger a, SignedInteger b,
	                             SignedInteger &result ) {
		static_assert( std::is_integral_v<SignedInteger> and
		                 std::is_signed_v<SignedInteger> and
		                 sizeof( SignedInteger ) <= 8U,
		               "Invalid signed integer" );
		auto const res64 =
		  static_cast<std::uint64_t>( a ) + static_cast<std::uint64_t>( b );
		(void)res64;
		result = static_cast<SignedInteger>( res64 );
		if constexpr( sizeof( SignedInteger ) < 8 ) {
			return result != res64;
		} else {
			return ( b > 0 and
			         a > ( std::numeric_limits<SignedInteger>::max( ) - b ) ) or
			       ( b < 0 and
			         a < ( std::numeric_limits<SignedInteger>::min( ) - b ) );
		}
	}

	template<typename SignedInteger>
	constexpr bool wrapping_sub( SignedInteger a, SignedInteger b,
	                             SignedInteger &result ) {
		static_assert( std::is_integral_v<SignedInteger> and
		                 std::is_signed_v<SignedInteger> and
		                 sizeof( SignedInteger ) <= 8U,
		               "Invalid signed integer" );
		auto const res64 =
		  static_cast<std::uint64_t>( a ) - static_cast<std::uint64_t>( b );
		(void)res64;
		result = static_cast<SignedInteger>( res64 );
		if constexpr( sizeof( SignedInteger ) < 8 ) {
			return result != res64;
		} else {
			if( b == 0 ) {
				return false;
			}
			if( b < 0 ) {
				return result < a;
			}
			// r > 0
			return result > a;
		}
	}

	DAW_ATTRIB_INLINE constexpr bool wrapping_sub( std::int16_t l, std::int16_t r,
	                                               std::int16_t &res ) noexcept {
		auto const res32 = l + r;
		res = static_cast<std::int16_t>( res32 );
		if( r == 0 ) {
			return false;
		}
		if( r < 0 ) {
			return res < l;
		}
		// r > 0
		return res > l;
	}

	DAW_ATTRIB_INLINE constexpr bool wrapping_sub( std::int32_t l, std::int32_t r,
	                                               std::int32_t &res ) noexcept {
		auto const l64 = l;
		auto const r64 = r;
		auto const res64 = l64 + r64;
		res = static_cast<std::int32_t>( res64 );
		if( r == 0 ) {
			return false;
		}
		if( r < 0 ) {
			return res < l;
		}
		// r > 0
		return res > l;
	}

	DAW_ATTRIB_INLINE constexpr bool wrapping_sub( std::int64_t l, std::int64_t r,
	                                               std::int64_t &res ) noexcept {
		auto const l64 = static_cast<std::uint64_t>( l );
		auto const r64 = static_cast<std::uint64_t>( r );
		auto const res64 = l64 - r64;
		res = static_cast<std::int64_t>( res64 );
		if( r == 0 ) {
			return false;
		}
		if( r < 0 ) {
			return res < l;
		}
		// r > 0
		return res > l;
	}

	inline constexpr struct checked_add_t {
		explicit checked_add_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			T result;
			if( DAW_UNLIKELY( wrapping_add( lhs, rhs, result ) ) ) {
				on_signed_integer_overflow( );
			}
			return result;
		}
	} checked_add{ };

	inline constexpr struct wrapped_add_t {
		explicit wrapped_add_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			T result;
			(void)wrapping_add( lhs, rhs, result );
			return result;
		}
	} wrapped_add{ };

	DAW_ATTRIB_INLINE constexpr bool wrapping_mul( std::int8_t l, std::int8_t r,
	                                               std::int8_t &res ) noexcept {
		std::uint32_t l32 = static_cast<unsigned char>( l );
		std::uint32_t r32 = static_cast<unsigned char>( r );
		auto res32 = l * r;
		res = static_cast<std::int8_t>( res32 );
		return res != res32;
	}

	DAW_ATTRIB_INLINE constexpr bool wrapping_mul( std::int16_t l, std::int16_t r,
	                                               std::int16_t &res ) noexcept {
		auto l32 = static_cast<std::uint32_t>( l );
		auto r32 = static_cast<std::uint32_t>( r );
		auto res32 = l32 * r32;
		res = static_cast<std::int16_t>( res32 );
		return res != res32;
	}

	DAW_ATTRIB_INLINE constexpr bool wrapping_mul( std::int32_t l, std::int32_t r,
	                                               std::int32_t &res ) noexcept {
		auto l64 = static_cast<std::uint64_t>( l );
		auto r64 = static_cast<std::uint64_t>( r );
		auto res64 = l64 * r64;
		res = static_cast<std::int32_t>( res64 );
		return res != res64;
	}

	DAW_ATTRIB_INLINE constexpr bool wrapping_mul( std::int64_t l, std::int64_t r,
	                                               std::int64_t &res ) noexcept {
		auto l64 = static_cast<std::uint64_t>( l );
		auto r64 = static_cast<std::uint64_t>( r );
		auto res64 = l64 * r64;
		res = static_cast<std::int64_t>( res64 );
		return l64 != 0 and res64 / l64 != r64;
	}

	inline constexpr struct checked_sub_t {
		explicit checked_sub_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			T result;
			if( DAW_UNLIKELY( wrapping_sub( lhs, rhs, result ) ) ) {
				on_signed_integer_overflow( );
			}
			return result;
		}
	} checked_sub{ };

	inline constexpr struct wrapped_sub_t {
		explicit wrapped_sub_t( ) = default;
		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			T result;
			(void)__builtin_sub_overflow( lhs, rhs, &result );
			return result;
		}
	} wrapped_sub{ };

	inline constexpr struct checked_mul_t {
		explicit checked_mul_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			T result;
			if( DAW_UNLIKELY( wrapping_mul( lhs, rhs, result ) ) ) {
				on_signed_integer_overflow( );
			}
			return result;
		}
	} checked_mul{ };

	inline constexpr struct wrapped_mul_t {
		explicit wrapped_mul_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			T result;
			(void)wrapping_mul( lhs, rhs, result );
			return result;
		}
	} wrapped_mul{ };

	inline constexpr struct checked_div_t {
		explicit checked_div_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			if( DAW_UNLIKELY( rhs == 0 ) ) {
				on_signed_integer_div_by_zero( );
				return lhs;
			} else if( rhs == T{ -1 } and lhs == std::numeric_limits<T>::min( ) ) {
				on_signed_integer_overflow( );
				return std::numeric_limits<T>::max( );
			}
			return lhs / rhs;
		}
	} checked_div{ };

	inline constexpr struct checked_rem_t {
		explicit checked_rem_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			if( DAW_UNLIKELY( rhs == 0 ) ) {
				on_signed_integer_div_by_zero( );
			}
			return lhs % rhs;
		}
	} checked_rem{ };

	inline constexpr struct checked_shl_t {
		explicit checked_shl_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			if( DAW_UNLIKELY( rhs == 0 ) ) {
				on_signed_integer_overflow( );
				return lhs;
			} else if( DAW_UNLIKELY( rhs >= ( sizeof( rhs ) * CHAR_BIT ) ) ) {
				on_signed_integer_overflow( );
				return lhs << ( sizeof( T ) * CHAR_BIT - 1 );
			}
			return lhs << rhs;
		}
	} checked_shl{ };

	inline constexpr struct checked_shr_t {
		explicit checked_shr_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			if( DAW_UNLIKELY( rhs == 0 ) ) {
				on_signed_integer_overflow( );
				return lhs;
			} else if( DAW_UNLIKELY( rhs >= ( sizeof( rhs ) * CHAR_BIT ) ) ) {
				on_signed_integer_overflow( );
				return lhs >> ( sizeof( T ) * CHAR_BIT - 1 );
			}
			return lhs >> rhs;
		}
	} checked_shr{ };
} // namespace daw::integers::sint_impl
#endif