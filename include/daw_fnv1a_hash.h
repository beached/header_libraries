// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace daw {
	namespace impl {
		using is_64bit_t = std::integral_constant<bool, sizeof( size_t ) == sizeof( uint64_t )>;

		constexpr size_t fnv_prime( ) noexcept {
			return impl::is_64bit_t::value ? 1099511628211ULL : 16777619UL;
		}

		constexpr size_t fnv_offset( ) noexcept {
			return impl::is_64bit_t::value ? 14695981039346656037ULL : 2166136261UL;
		}

	} // namespace impl

	template<typename T>
	struct fnv1a_hash_t {
		constexpr size_t operator( )( T const *const ptr ) noexcept {
			auto hash = daw::impl::fnv_offset( );
			auto bptr = static_cast<uint8_t const *const>( static_cast<void const *const>( ptr ) );
			for( size_t n = 0; n < sizeof( T ); ++n ) {
				hash = hash ^ static_cast<size_t>( bptr[n] );
				hash *= daw::impl::fnv_prime( );
			}
			return hash;
		}
	};

	template<typename T>
	constexpr size_t fnv1a_hash( T const &value ) {
		return fnv1a_hash_t<T>{}( &value );
	}

	constexpr size_t fnv1a_hash( char const *ptr ) {
		auto hash = daw::impl::fnv_offset( );
		while( *ptr != 0 ) {
			hash = hash ^ static_cast<size_t>( *ptr );
			hash *= daw::impl::fnv_prime( );
			++ptr;
		}
		return hash;
	}

	template<typename CharT>
	constexpr size_t fnv1a_hash( CharT const *ptr, size_t len ) {
		auto hash = daw::impl::fnv_offset( );
		while( len-- > 0 ) {
			hash = hash ^ static_cast<size_t>( *ptr );
			hash *= daw::impl::fnv_prime( );
			++ptr;
		}
		return hash;
	}

	template<size_t sz>
	constexpr size_t fnv1a_hash( char const ( &ptr )[sz] ) {
		auto hash = daw::impl::fnv_offset( );
		for( size_t n = 0; n < sz; ++n ) {
			hash = hash ^ static_cast<size_t>( ptr[n] );
			hash *= daw::impl::fnv_prime( );
		}
		return hash;
	}
} // namespace daw
