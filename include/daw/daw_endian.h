// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_arith_traits.h"
#include "daw_constant.h"
#include "daw_cpp_feature_check.h"

#include <cstddef>
#include <cstdint>
#include <daw/stdinc/enable_if.h>

namespace daw {
	enum class endian {
#if defined( DAW_HAS_MSVC )
		little = 1234,
		big = 4321,
		native = little
#else
		little = __ORDER_LITTLE_ENDIAN__,
		big = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
#endif
	};

	namespace endian_details {
		template<typename T>
		constexpr T swap_bytes( T value,
		                        daw::constant<sizeof( std::uint8_t )> ) noexcept {
			return value;
		}

		template<typename T>
		constexpr T swap_bytes( T value,
		                        daw::constant<sizeof( std::uint16_t )> ) noexcept {
			return static_cast<T>(
			  static_cast<std::uint16_t>( ( static_cast<std::uint16_t>( value ) >>
			                                static_cast<std::uint16_t>( 8U ) ) &
			                              static_cast<std::uint16_t>( 0x00FFU ) ) |
			  static_cast<std::uint16_t>( ( static_cast<std::uint16_t>( value )
			                                << static_cast<std::uint16_t>( 8U ) ) &
			                              static_cast<std::uint16_t>( 0xFF00U ) ) );
		}

		template<typename T>
		constexpr T swap_bytes( T value,
		                        daw::constant<sizeof( std::uint32_t )> ) noexcept {
			return static_cast<T>( static_cast<std::uint32_t>(
			                         ( static_cast<std::uint32_t>( value ) >>
			                           static_cast<std::uint32_t>( 24U ) ) &
			                         static_cast<std::uint32_t>( 0x0000'00FFU ) ) |
			                       static_cast<std::uint32_t>(
			                         ( static_cast<std::uint32_t>( value ) >>
			                           static_cast<std::uint32_t>( 8U ) ) &
			                         static_cast<std::uint32_t>( 0x0000'FF00U ) ) |
			                       static_cast<std::uint32_t>(
			                         ( static_cast<std::uint32_t>( value )
			                           << static_cast<std::uint32_t>( 8U ) ) &
			                         static_cast<std::uint32_t>( 0x00FF'0000U ) ) |
			                       static_cast<std::uint32_t>(
			                         ( static_cast<std::uint32_t>( value )
			                           << static_cast<std::uint32_t>( 24U ) ) &
			                         static_cast<std::uint32_t>( 0xFF00'0000U ) ) );
		}

		template<typename T>
		constexpr T swap_bytes( T value,
		                        daw::constant<sizeof( std::uint64_t )> ) noexcept {
			return static_cast<T>(
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value ) >>
			      static_cast<std::uint64_t>( 56U ) ) &
			    static_cast<std::uint64_t>( 0x0000'0000'0000'00FFU ) ) |
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value ) >>
			      static_cast<std::uint64_t>( 40U ) ) &
			    static_cast<std::uint64_t>( 0x0000'0000'0000'FF00U ) ) |
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value ) >>
			      static_cast<std::uint64_t>( 24U ) ) &
			    static_cast<std::uint64_t>( 0x0000'0000'00FF'0000U ) ) |
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value ) >>
			      static_cast<std::uint64_t>( 8U ) ) &
			    static_cast<std::uint64_t>( 0x0000'0000'FF00'0000U ) ) |
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value )
			      << static_cast<std::uint64_t>( 8U ) ) &
			    static_cast<std::uint64_t>( 0x0000'00FF'0000'0000U ) ) |
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value )
			      << static_cast<std::uint64_t>( 24U ) ) &
			    static_cast<std::uint64_t>( 0x0000'FF00'0000'0000U ) ) |
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value )
			      << static_cast<std::uint64_t>( 40U ) ) &
			    static_cast<std::uint64_t>( 0x00FF'0000'0000'0000U ) ) |
			  static_cast<std::uint64_t>(
			    ( static_cast<std::uint64_t>( value )
			      << static_cast<std::uint64_t>( 56U ) ) &
			    static_cast<std::uint64_t>( 0xFF00'0000'0000'0000U ) ) );
		}
	} // namespace endian_details

	template<typename T,
	         std::enable_if_t<daw::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T to_little_endian( T value ) noexcept {
		if constexpr( endian::native == endian::little ) {
			return value;
		} else {
			return endian_details::swap_bytes( value, daw::constant<sizeof( T )>{ } );
		}
	}

	template<typename T,
	         std::enable_if_t<daw::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T to_big_endian( T value ) noexcept {
		if constexpr( endian::native == endian::big ) {
			return value;
		} else {
			return endian_details::swap_bytes( value, daw::constant<sizeof( T )>{ } );
		}
	}

	template<typename T,
	         std::enable_if_t<daw::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T network_to_host_endian( T value ) noexcept {
		if constexpr( endian::native == endian::big ) {
			return value;
		} else {
			return to_little_endian( value );
		}
	}

	template<endian SourceEndian, typename T,
	         std::enable_if_t<daw::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr T to_native_endian( T value ) noexcept {
		if constexpr( SourceEndian == endian::native ) {
			return value;
		} else if constexpr( endian::native == endian::little ) {
			return to_little_endian( value );
		} else {
			return to_big_endian( value );
		}
	}
} // namespace daw
