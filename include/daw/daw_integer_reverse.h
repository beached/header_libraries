// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_attributes.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/traits/daw_traits_is_one_of.h"

#include <cstdint>
#include <type_traits>

namespace daw::integers {
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint8_t
	reverse_bits( std::uint8_t value ) {
#if not DAW_HAS_CLANG_VER_LT( 10, 0 ) and __has_builtin( __builtin_bitreverse8 )
		return __builtin_bitreverse8( value );
#else
		auto x = static_cast<unsigned>( value );
		x = ( ( x & 0x1 ) << 7 ) | ( ( x & 0x2 ) << 5 ) | ( ( x & 0x4 ) << 3 ) |
		    ( ( x & 0x8 ) << 1 ) | ( ( x & 0x10 ) >> 1 ) | ( ( x & 0x20 ) >> 3 ) |
		    ( ( x & 0x40 ) >> 5 ) | ( ( x & 0x80 ) >> 7 );
		return static_cast<std::uint8_t>( x );
#endif
	}

	[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint16_t
	reverse_bits( std::uint16_t value ) {
#if not DAW_HAS_CLANG_VER_LT( 10, 0 ) and \
  __has_builtin( __builtin_bitreverse16 )
		return __builtin_bitreverse16( value );
#else
		auto a = static_cast<unsigned>( value );
		a = ( ( a & 0xFF00 ) >> 8 ) | ( ( a & 0x00FF ) << 8 );
		a = ( ( a & 0xF0F0 ) >> 4 ) | ( ( a & 0x0F0F ) << 4 );
		a = ( ( a & 0xCCCC ) >> 2 ) | ( ( a & 0x3333 ) << 2 );
		a = ( ( a & 0xAAAA ) >> 1 ) | ( ( a & 0x5555 ) << 1 );
		return static_cast<std::uint16_t>( a );
#endif
	}

	[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint32_t
	reverse_bits( std::uint32_t value ) {
#if not DAW_HAS_CLANG_VER_LT( 10, 0 ) and \
  __has_builtin( __builtin_bitreverse32 )
		return __builtin_bitreverse32( value );
#else
		value = ( ( value & 0x55555555 ) << 1 ) | ( ( value >> 1 ) & 0x55555555 );
		value = ( ( value & 0x33333333 ) << 2 ) | ( ( value >> 2 ) & 0x33333333 );
		value = ( ( value & 0x0F0F0F0F ) << 4 ) | ( ( value >> 4 ) & 0x0F0F0F0F );
		value = ( value << 24 ) | ( ( value & 0xFF00 ) << 8 ) |
		        ( ( value >> 8 ) & 0xFF00 ) | ( value >> 24 );
		return value;
#endif
	}

	[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::uint64_t
	reverse_bits( std::uint64_t value ) {
#if not DAW_HAS_CLANG_VER_LT( 10, 0 ) and \
  __has_builtin( __builtin_bitreverse64 )
		return __builtin_bitreverse64( value );
#else
		value = ( ( value >> 1 ) & 0x5555555555555555 ) |
		        ( ( value & 0x5555555555555555 ) << 1 );
		value = ( ( value >> 2 ) & 0x3333333333333333 ) |
		        ( ( value & 0x3333333333333333 ) << 2 );
		value = ( ( value >> 4 ) & 0x0f0f0f0f0f0f0f0f ) |
		        ( ( value & 0x0f0f0f0f0f0f0f0f ) << 4 );
		value = ( ( value >> 8 ) & 0x00ff00ff00ff00ff ) |
		        ( ( value & 0x00ff00ff00ff00ff ) << 8 );
		value = ( ( value >> 16 ) & 0x0000ffff0000ffff ) |
		        ( ( value & 0x0000ffff0000ffff ) << 16 );
		value = ( value >> 32 ) | ( value << 32 );
		return value;
#endif
	}

	template<typename..., typename T,
	         std::enable_if_t<
	           not daw::traits::is_one_of_v<T, std::uint8_t, std::uint16_t,
	                                        std::uint32_t, std::uint64_t>,
	           std::nullptr_t> = nullptr>
	[[nodiscard]] DAW_ATTRIB_NOINLINE constexpr T reverse_bits( T ) {
		static_assert( daw::traits::is_one_of_v<T, std::uint8_t, std::uint16_t,
		                                        std::uint32_t, std::uint64_t>,
		               "Unsupported integer type for bit reversal" );
	}
} // namespace daw::integers
