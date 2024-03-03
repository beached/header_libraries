// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#if defined( __clang__ ) or defined( __GNUC__ )

#include "daw/daw_arith_traits.h"
#include "daw/daw_attributes.h"
#include "daw/daw_consteval.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_likely.h"
#include "daw/daw_traits.h"
#include "daw_signed_error_handling.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <exception>
#include <limits>
#include <type_traits>
#include <utility>

namespace daw::integers::sint_impl {
	template<typename T>
	inline constexpr bool is_valid_int_type =
	  std::is_integral_v<T> and std::is_signed_v<T> and
	  sizeof( T ) <= sizeof( std::int64_t );

	template<typename SignedInteger,
	         std::enable_if_t<
	           daw::traits::is_one_of_v<SignedInteger, signed char, short>,
	           std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool
	wrapping_add( SignedInteger a, SignedInteger b, SignedInteger &result ) {
		static_assert( sizeof( int ) >= sizeof( SignedInteger ) );
		int const r2 = a + b;
		bool const r = not daw::in_range<SignedInteger>( r2 );
		result = static_cast<SignedInteger>( r2 );
		return r;
	}

	template<typename SignedInteger,
	         std::enable_if_t<
	           daw::traits::is_one_of_v<SignedInteger, int, long, long long>,
	           std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool
	wrapping_add( SignedInteger a, SignedInteger b, SignedInteger &result ) {
		return __builtin_add_overflow( a, b, &result );
	}

	template<typename SignedInteger,
	         std::enable_if_t<
	           daw::traits::is_one_of_v<SignedInteger, signed char, short>,
	           std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool
	wrapping_sub( SignedInteger a, SignedInteger b, SignedInteger &result ) {
		static_assert( sizeof( int ) >= sizeof( SignedInteger ) );
		int const r2 = a - b;
		bool const r = not daw::in_range<SignedInteger>( r2 );
		result = static_cast<SignedInteger>( r2 );
		return r;
	}

	template<typename SignedInteger,
	         std::enable_if_t<
	           daw::traits::is_one_of_v<SignedInteger, int, long, long long>,
	           std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool
	wrapping_sub( SignedInteger a, SignedInteger b, SignedInteger &result ) {
		return __builtin_sub_overflow( a, b, &result );
	}

	template<typename SignedInteger,
	         std::enable_if_t<
	           daw::traits::is_one_of_v<SignedInteger, signed char, short>,
	           std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool
	wrapping_mul( SignedInteger a, SignedInteger b, SignedInteger &result ) {
		static_assert( sizeof( int ) >= sizeof( SignedInteger ) );
		int const r2 = a * b;
		bool const r = not daw::in_range<SignedInteger>( r2 );
		result = static_cast<SignedInteger>( r2 );
		return r;
	}

	template<typename SignedInteger,
	         std::enable_if_t<
	           daw::traits::is_one_of_v<SignedInteger, int, long, long long>,
	           std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE constexpr bool
	wrapping_mul( SignedInteger a, SignedInteger b, SignedInteger &result ) {
		return __builtin_mul_overflow( a, b, &result );
	}

	inline constexpr struct checked_div_t {
		explicit checked_div_t( ) = default;

		template<typename T,
		         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
			if( DAW_UNLIKELY( rhs == 0 ) ) {
				on_signed_integer_div_by_zero( );
				return lhs;
			}
			if constexpr( sizeof( T ) == 8 ) {
#if defined( DAW_HAS_INT128 )
				auto const l = static_cast<daw::int128_t>( lhs );
				auto const r = static_cast<daw::int128_t>( rhs );
				auto const res128 = l / r;
				auto const res = static_cast<T>( res128 );
				if( DAW_UNLIKELY( res != res128 ) ) {
					on_signed_integer_overflow( );
				}
				return res;
#else
				if( DAW_UNLIKELY( rhs == -1 and
				                  lhs == std::numeric_limits<T>::min( ) ) ) {
					on_signed_integer_overflow( );
					return std::numeric_limits<T>::max( );
				}
				return lhs / rhs;
#endif
			} else {
				static_assert( sizeof( T ) < 8 );
				auto const l = static_cast<std::int64_t>( lhs );
				auto const r = static_cast<std::int64_t>( rhs );
				auto const res64 = l / r;
				auto const res = static_cast<T>( res64 );
				if( DAW_UNLIKELY( res != res64 ) ) {
					on_signed_integer_overflow( );
				}
				return res;
			}
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
			if( lhs == std::numeric_limits<T>::min( ) and rhs.value( ) == T{ -1 } ) {
				on_signed_integer_overflow( );
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