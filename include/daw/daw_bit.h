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
#include "daw/daw_bit_count.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"

#include <cstddef>
#include <limits>
#include <type_traits>

namespace daw {
	template<typename T>
	[[deprecated( "Use mask_msb" )]] constexpr T
	get_left_mask( size_t left_zero_bits ) noexcept {
		return static_cast<T>( max_value<T> >> left_zero_bits );
	}

	template<typename T>
	constexpr T mask_msb( size_t bit_count ) noexcept {
		if( bit_count >= daw::bsizeof<T> ) {
			return static_cast<T>( 0 );
		}
		return static_cast<T>( max_value<T> >> bit_count );
	}

	template<typename T>
	[[deprecated( "Use mask_lsb" )]] constexpr T
	get_right_mask( size_t right_zero_bits ) noexcept {
		return static_cast<T>( max_value<T> << right_zero_bits );
	}

	template<typename T>
	constexpr T mask_lsb( size_t bit_count ) noexcept {
		if( bit_count >= daw::bsizeof<T> ) {
			return static_cast<T>( 0 );
		}
		return static_cast<T>( max_value<T> << bit_count );
	}

	template<typename Bit, typename... Bits>
	constexpr auto make_mask( Bit bit, Bits... bits ) noexcept {
		static_assert( std::is_integral_v<Bit>,
		               "Only integer types are supported" );

		using expand = int[];
		static_cast<void>( expand{ 0, ( ( bit |= bits ), 0 )... } );
		return bit;
	}

	template<typename Integer, typename MaskBit, typename... MaskBits>
	constexpr bool are_set( Integer value, MaskBit mask_bit,
	                        MaskBits... mask_bits ) noexcept {
		static_assert( std::is_integral_v<Integer>,
		               "Only integer types are supported" );
		static_assert( std::is_integral_v<MaskBit>,
		               "Only integer types are supported" );

		using common_t = std::common_type_t<Integer, MaskBit, MaskBits...>;

		return ( static_cast<common_t>( value ) &
		         make_mask( static_cast<common_t>( mask_bit ),
		                    static_cast<common_t>( mask_bits )... ) ) !=
		       static_cast<common_t>( 0 );
	}

	/// @brief set bits at positions specified by b,bs...
	template<typename Integer, typename Bit, typename... Bits>
	constexpr Integer set_bits( Integer i, Bit b, Bits... bs ) noexcept {
		constexpr Integer one = static_cast<Integer>( 1u );
		auto result = static_cast<Integer>( one << static_cast<Integer>( b ) );
		result = ( ( one << static_cast<Integer>( bs ) ) | ... | result );
		return i | result;
	}

	/// @brief set bits at positions specified by b,bs...
	template<typename Integer, typename Bit, typename... Bits>
	constexpr Integer unset_bits( Integer i, Bit b, Bits... bs ) noexcept {
		constexpr Integer one = static_cast<Integer>( 1u );
		auto result = static_cast<Integer>( one << static_cast<Integer>( b ) );
		result = ( ( one << static_cast<Integer>( bs ) ) | ... | result );
		return i & ~result;
	}

	/// @brief get value with all bits but those specified masked out
	template<typename Integer, typename Bit, typename... Bits>
	constexpr Integer get_bits( Integer i, Bit b, Bits... bs ) noexcept {
		constexpr Integer one = static_cast<Integer>( 1u );
		auto result = static_cast<Integer>( one << static_cast<Integer>( b ) );
		result = ( ( one << static_cast<Integer>( bs ) ) | ... | result );
		return i & result;
	}

} // namespace daw
