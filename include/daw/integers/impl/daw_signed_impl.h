// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_signed_clanggcc.h"
#include "daw_signed_msvc.h"

#include <cstdint>
#include <type_traits>

namespace daw::integers::sint_impl {
	template<typename Unsigned, std::size_t... Is>
	DAW_ATTRIB_INLINE constexpr Unsigned
	from_bytes_le( unsigned char const *ptr, std::index_sequence<Is...> ) {
		constexpr auto f = []( unsigned char c, size_t n ) {
			return static_cast<Unsigned>( c ) << ( 8U * n );
		};
		auto result = Unsigned{ ( f( ptr[Is], Is ) | ... ) };
		return result;
	}

	template<typename Unsigned, std::size_t... Is>
	DAW_ATTRIB_INLINE constexpr Unsigned
	from_bytes_be( unsigned char const *ptr, std::index_sequence<Is...> ) {
		constexpr auto StartVal = sizeof( Unsigned ) - 1;
		constexpr auto f = []( unsigned char c, size_t n ) {
			return static_cast<Unsigned>( c ) << ( 8U * n );
		};
		auto result = Unsigned{ ( f( ptr[StartVal - Is], Is ) | ... ) };
		return result;
	}

	template<typename Lhs, typename Rhs>
	inline constexpr bool size_fits_v = sizeof( Lhs ) <= sizeof( Rhs );

	template<typename SignedInteger, typename Integer>
	inline constexpr bool convertible_signed_int =
	  std::is_integral_v<Integer> and std::is_signed_v<Integer> and
	  ( sizeof( Integer ) <= sizeof( SignedInteger ) );

	inline constexpr struct debug_checked_add_t {
		explicit debug_checked_add_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_add( lhs, rhs );
#elif DAW_DEFAULT_SIGNED_CHECKING == 2
			return wrapped_add( lhs, rhs );
#else
			return lhs + rhs;
#endif
		}
	} debug_checked_add{ };

	inline constexpr struct debug_checked_sub_t {
		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_sub( lhs, rhs );
#elif DAW_DEFAULT_SIGNED_CHECKING == 2
			return wrapped_sub( lhs, rhs );
#else
			return lhs - rhs;
#endif
		}
	} debug_checked_sub{ };

	inline constexpr struct debug_checked_mul_t {
		explicit debug_checked_mul_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_mul( lhs, rhs );
#elif DAW_DEFAULT_SIGNED_CHECKING == 2
			return wrapped_mul( lhs, rhs );
#else
			return lhs * rhs;
#endif
		}
	} debug_checked_mul{ };

	inline constexpr struct debug_checked_div_t {
		explicit debug_checked_div_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_div( lhs, rhs );
#else
			return lhs / rhs;
#endif
		}
	} debug_checked_div{ };

	inline constexpr struct debug_checked_rem_t {
		explicit debug_checked_rem_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_rem( lhs, rhs );
#else
			return lhs % rhs;
#endif
		}
	} debug_checked_rem{ };

	inline constexpr struct checked_neg_t {
		explicit checked_neg_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs ) const {
			if constexpr( sizeof( T ) < 4 ) {
				return static_cast<T>( -static_cast<std::int32_t>( lhs ) );
			} else if constexpr( sizeof( T ) < 8 ) {
				return static_cast<T>( -static_cast<std::int64_t>( lhs ) );
			} else {
				return checked_mul( lhs, T{ -1 } );
			}
		}
	} checked_neg{ };

	inline constexpr struct debug_checked_neg_t {
		explicit debug_checked_neg_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs ) const {

#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_neg( lhs );
#else
			return -lhs;
#endif
		}
	} debug_checked_neg{ };

	inline constexpr struct debug_checked_shl_t {
		explicit debug_checked_shl_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_shl( lhs, rhs );
#else
			return lhs << rhs;
#endif
		}
	} debug_checked_shl{ };
	inline constexpr struct debug_checked_shr_t {
		explicit debug_checked_shr_t( ) = default;

		template<typename T,
		         std::enable_if_t<sint_impl::size_fits_v<T, std::int64_t>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr T operator( )( T lhs, T rhs ) const {
#if DAW_DEFAULT_SIGNED_CHECKING == 0
			return checked_shr( lhs, rhs );
#else
			return lhs >> rhs;
#endif
		}
	} debug_checked_shr{ };
} // namespace daw::integers::sint_impl
