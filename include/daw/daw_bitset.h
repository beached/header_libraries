// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_consteval.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace daw {
	template<std::size_t BitCount>
	class bitset {
		static_assert( BitCount > 2, "BitCount must be greater than 2" );
		using limb_t = std::uint32_t;
		static constexpr std::size_t limb_size = sizeof( limb_t ) * 8;
		static constexpr std::size_t num_limbs = ( BitCount - 1 ) / limb_size + 1;
		limb_t m_bits[num_limbs]{ };

		DAW_ATTRIB_INLINE static constexpr std::size_t
		get_limb( std::size_t index ) {
			assert( index < size( ) );
			return index / limb_size;
		}

		DAW_ATTRIB_INLINE static constexpr limb_t get_mask( std::size_t index ) {
			return limb_t{ 1 } << ( index % limb_size );
		}

	public:
		bitset( ) = default;

		static DAW_CONSTEVAL std::size_t size( ) {
			return BitCount;
		}

		DAW_ATTRIB_INLINE constexpr void unset( std::size_t index ) {
			assert( index < size( ) );
			m_bits[get_limb( index )] &= ~( 1u << index % 4 );
			auto const limb = get_limb( index );
			auto const mask = get_mask( index );
			m_bits[limb] &= ~mask;
		}

		DAW_ATTRIB_INLINE constexpr void set( std::size_t index ) {
			assert( index < size( ) );
			auto const limb = get_limb( index );
			auto const mask = get_mask( index );
			m_bits[limb] |= mask;
		}

		DAW_ATTRIB_INLINE constexpr void set( std::size_t index, bool value ) {
			assert( index < size( ) );
			if( value ) {
				set( index );
			} else {
				unset( index );
			}
		}

		DAW_ATTRIB_INLINE constexpr bool test( std::size_t index ) const {
			assert( index < size( ) );
			auto const limb = get_limb( index );
			auto const mask = get_mask( index );
			return m_bits[limb] & mask;
		}
	};
} // namespace daw
