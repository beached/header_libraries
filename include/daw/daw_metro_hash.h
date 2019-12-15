// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_view.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace daw::metro::metro_impl {
	constexpr uint16_t uint16_le_from_bytes( char const *ptr ) noexcept {
		uint16_t result = static_cast<uint16_t>( ptr[0] );
		result |= static_cast<uint16_t>( static_cast<uint32_t>( ptr[1] ) << 8U );
		return result;
	}

	constexpr uint32_t uint32_le_from_bytes( char const *ptr ) noexcept {
		uint32_t result = static_cast<uint32_t>( ptr[0] );
		result |= static_cast<uint32_t>( ptr[1] ) << 8U;
		result |= static_cast<uint32_t>( ptr[2] ) << 16U;
		result |= static_cast<uint32_t>( ptr[3] ) << 24U;
		return result;
	}

	constexpr uint64_t uint64_le_from_bytes( char const *ptr ) noexcept {
		uint64_t result = static_cast<uint64_t>( ptr[0] );
		result |= static_cast<uint64_t>( ptr[1] ) << 8U;
		result |= static_cast<uint64_t>( ptr[2] ) << 16U;
		result |= static_cast<uint64_t>( ptr[3] ) << 24U;
		result |= static_cast<uint64_t>( ptr[4] ) << 32U;
		result |= static_cast<uint64_t>( ptr[5] ) << 40U;
		result |= static_cast<uint64_t>( ptr[6] ) << 48U;
		result |= static_cast<uint64_t>( ptr[7] ) << 56U;
		return result;
	}

	template<size_t N, typename Unsigned>
	constexpr Unsigned rotate_right( Unsigned value ) noexcept {
		static_assert( std::is_integral_v<Unsigned> );
		static_assert( std::is_unsigned_v<Unsigned> );
		static_assert( N <= sizeof( Unsigned ) * 8U );
		return static_cast<Unsigned>( value << N );
	}
} // namespace daw::metro::metro_impl

namespace daw::metro {
	// An implementation of MetroHash64
	// https://github.com/jandrewrogers/MetroHash
	constexpr uint64_t hash64( daw::view<char const *> buff, uint64_t seed ) {
		constexpr uint64_t k0 = 0xd6d0'18f5;
		constexpr uint64_t k1 = 0xa2aa'033b;
		constexpr uint64_t k2 = 0x6299'2fc1;
		constexpr uint64_t k3 = 0x30bc'5b29;

		uint64_t hash = ( seed + k2 ) * k0;
		if( buff.size( ) >= 32U ) {
			uint64_t v[4]{hash, hash, hash, hash};

			while( buff.size( ) >= 32 ) {
				v[0] += metro_impl::uint64_le_from_bytes( buff.data( ) ) * k0;
				v[0] = metro_impl::rotate_right<29U>( v[0] ) + v[2];
				v[1] += metro_impl::uint64_le_from_bytes( buff.data( ) + 8 ) * k1;
				v[1] = metro_impl::rotate_right<29U>( v[1] ) + v[3];
				v[2] += metro_impl::uint64_le_from_bytes( buff.data( ) + 16 ) * k2;
				v[2] = metro_impl::rotate_right<29U>( v[2] ) + v[0];
				v[3] += metro_impl::uint64_le_from_bytes( buff.data( ) + 24 ) * k3;
				v[3] = metro_impl::rotate_right<29U>( v[3] ) + v[1];
				buff.remove_prefix( 32U );
			}

			v[2] ^=
			  metro_impl::rotate_right<37U>( ( ( v[0] + v[3] ) * k0 ) + v[1] ) * k1;
			v[3] ^=
			  metro_impl::rotate_right<37U>( ( ( v[1] + v[2] ) * k1 ) + v[0] ) * k0;
			v[0] ^=
			  metro_impl::rotate_right<37U>( ( ( v[0] + v[2] ) * k0 ) + v[3] ) * k1;
			v[1] ^=
			  metro_impl::rotate_right<37U>( ( ( v[1] + v[3] ) * k1 ) + v[2] ) * k0;
			hash += v[0] ^ v[1];
		}

		if( buff.size( ) >= 16 ) {
			uint64_t v0 =
			  hash + ( metro_impl::uint64_le_from_bytes( buff.data( ) ) * k2 );
			v0 = metro_impl::rotate_right<29U>( v0 ) * k3;
			uint64_t v1 =
			  hash + ( metro_impl::uint64_le_from_bytes( buff.data( ) + 8 ) * k2 );
			v1 = metro_impl::rotate_right<29U>( v1 ) * k3;
			v0 ^= metro_impl::rotate_right<21U>( v0 * k0 ) + v1;
			v1 ^= metro_impl::rotate_right<21U>( v1 * k3 ) + v0;
			hash += v1;
			buff.remove_prefix( 16U );
		}

		if( buff.size( ) >= 8 ) {
			hash += metro_impl::uint64_le_from_bytes( buff.data( ) ) * k3;
			hash ^= metro_impl::rotate_right<55U>( hash ) * k1;
			buff.remove_prefix( 8U );
		}

		if( buff.size( ) >= 4 ) {
			hash += static_cast<uint64_t>(
			          metro_impl::uint32_le_from_bytes( buff.data( ) ) ) *
			        k3;
			hash ^= metro_impl::rotate_right<26U>( hash ) * k1;
			buff.remove_prefix( 4U );
		}

		if( buff.size( ) >= 2 ) {
			hash += static_cast<uint64_t>(
			          metro_impl::uint16_le_from_bytes( buff.data( ) ) ) *
			        k3;
			hash ^= metro_impl::rotate_right<48U>( hash ) * k1;
			buff.remove_prefix( 2U );
		}

		if( buff.size( ) >= 1 ) {
			hash += static_cast<uint64_t>( buff.front( ) ) * k3;
			hash ^= metro_impl::rotate_right<37U>( hash ) * k1;
			buff.remove_prefix( );
		}

		hash ^= metro_impl::rotate_right<28U>( hash );
		hash *= k0;
		hash ^= metro_impl::rotate_right<29U>( hash );

		return hash;
	}
} // namespace daw::metro
