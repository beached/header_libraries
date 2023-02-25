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

	namespace {
		inline constexpr struct checked_add_t {
			explicit checked_add_t( ) = default;

			template<typename T,
			         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
				T result;
				if( DAW_UNLIKELY( __builtin_add_overflow( lhs, rhs, &result ) ) ) {
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
				(void)__builtin_add_overflow( lhs, rhs, &result );
				return result;
			}
		} wrapped_add{ };

		inline constexpr struct checked_sub_t {
			explicit checked_sub_t( ) = default;

			template<typename T,
			         std::enable_if_t<is_valid_int_type<T>, std::nullptr_t> = nullptr>
			DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
				T result;
				if( DAW_UNLIKELY( __builtin_sub_overflow( lhs, rhs, &result ) ) ) {
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
				if( DAW_UNLIKELY( __builtin_mul_overflow( lhs, rhs, &result ) ) ) {
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
				(void)__builtin_mul_overflow( lhs, rhs, &result );
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
	} // namespace
} // namespace daw::integers::sint_impl
#endif