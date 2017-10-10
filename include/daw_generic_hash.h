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
#include <string>

#include "daw_traits.h"

namespace daw {
	namespace impl {
		constexpr bool is_64bit_v = sizeof( size_t ) == sizeof( uint64_t );
		constexpr size_t preferred_size = is_64bit_v ? 8 : 4;
	} // namespace impl

	template<size_t HashBytes>
	struct generic_hash_t;

	template<>
	struct generic_hash_t<4> {
		using result_t = uint32_t;
		static constexpr size_t const hash_size = 4;
		static constexpr result_t const hash_init = 2166136261UL;

		template<typename Value, std::enable_if_t<is_integral_v<Value>, std::nullptr_t> = nullptr>
		static constexpr result_t append_hash( result_t current_hash, Value const &value ) noexcept {
			result_t const fnv1a_prime = 16777619UL;
			for( size_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<result_t>(
				  ( static_cast<uintmax_t>( value ) & ( static_cast<uintmax_t>( 0xFF ) << ( n * 8u ) ) ) >> ( n * 8u ) );
				current_hash *= fnv1a_prime;
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2,
		         std::enable_if_t<is_integral_v<typename std::iterator_traits<Iterator1>::type>, std::nullptr_t> = nullptr>
		constexpr result_t operator( )( Iterator1 first, Iterator2 const last ) const noexcept {
			auto hash = hash_init;
			while( first != last ) {
				hash = append_hash( hash, *first );
			}
			return hash;
		}
		template<typename Member, size_t N, std::enable_if_t<is_integral_v<Member>, std::nullptr_t> = nullptr>
		constexpr result_t operator( )( Member const ( &member )[N] ) const noexcept {
			return operator( )( member, std::next( member, static_cast<intmax_t>( N ) ) );
		}

		template<typename Integral, std::enable_if_t<is_integral_v<Integral>, std::nullptr_t> = nullptr>
		constexpr result_t operator( )( Integral const value ) const noexcept {
			return append_hash( hash_init, value );
		}
	};
	template<>
	struct generic_hash_t<8> {
		using result_t = uint64_t;
		static constexpr size_t const hash_size = 8;
		static constexpr result_t const hash_init = 14695981039346656037ULL;

		template<typename Value, std::enable_if_t<is_integral_v<Value>, std::nullptr_t> = nullptr>
		static constexpr result_t append_hash( result_t current_hash, Value const &value ) noexcept {
			result_t const fnv1a_prime = 1099511628211ULL;
			for( size_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<result_t>(
				  ( static_cast<uintmax_t>( value ) & ( static_cast<uintmax_t>( 0xFF ) << ( n * 8u ) ) ) >> ( n * 8u ) );
				current_hash *= fnv1a_prime;
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2,
		         std::enable_if_t<is_integral_v<typename std::iterator_traits<Iterator1>::type>, std::nullptr_t> = nullptr>
		constexpr result_t operator( )( Iterator1 first, Iterator2 const last ) const noexcept {
			auto hash = hash_init;
			while( first != last ) {
				hash = append_hash( hash, *first );
			}
			return hash;
		}
		template<typename Member, size_t N, std::enable_if_t<is_integral_v<Member>, std::nullptr_t> = nullptr>
		constexpr result_t operator( )( Member const ( &member )[N] ) const noexcept {
			return operator( )( member, std::next( member, static_cast<intmax_t>( N ) ) );
		}

		template<typename Integral, std::enable_if_t<is_integral_v<Integral>, std::nullptr_t> = nullptr>
		constexpr result_t operator( )( Integral const value ) const noexcept {
			return append_hash( hash_init, value );
		}

		template<typename T>
		constexpr result_t operator( )( T const *const ptr ) const noexcept {
			auto hash = hash_init;
			auto bptr = static_cast<uint8_t const *const>( static_cast<void const *const>( ptr ) );
			for( size_t n = 0; n < sizeof( T ); ++n ) {
				append_hash( hash, bptr[n] );
			}
			return hash;
		}
	};

	template<size_t HashBytes = impl::preferred_size, typename T,
	         std::enable_if_t<is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr auto generic_hash( T const value ) noexcept {
		return generic_hash_t<HashBytes>{}( value );
	}

	template<size_t HashBytes = impl::preferred_size, typename Iterator, typename IteratorL>
	constexpr auto generic_hash( Iterator first, IteratorL const last ) noexcept {
		using hash_t = generic_hash_t<HashBytes>;
		auto hash = hash_t::hash_init;
		while( first != last ) {
			hash = hash_t::append_hash( hash, *first );
			++first;
		}
		return hash;
	}

	template<size_t HashBytes = impl::preferred_size, typename Iterator>
	constexpr auto generic_hash( Iterator first, size_t const len ) noexcept {
		using hash_t = generic_hash_t<HashBytes>;
		auto hash = hash_t::hash_init;
		for( size_t n=0; n<len; ++n ) {
			hash = hash_t::append_hash( hash, *first );
			++first;
		}
		return hash;
	}

	template<size_t HashBytes = impl::preferred_size, typename CharT, typename Traits, typename Allocator>
	auto generic_hash( std::basic_string<CharT, Traits, Allocator> const &str ) noexcept {
		return generic_hash<HashBytes>( str.data( ), str.size( ) );
	}

	template<size_t HashBytes = impl::preferred_size, typename CharT, typename Traits, typename Allocator>
	auto generic_hash( std::basic_string<CharT, Traits, Allocator> &&str ) noexcept {
		auto tmp = std::move( str );
		return generic_hash<HashBytes>( tmp.data( ), tmp.size( ) );
	}

	template<size_t HashBytes = impl::preferred_size, size_t N>
	constexpr auto generic_hash( char const ( &ptr )[N] ) noexcept {
		return generic_hash<HashBytes>( ptr, N );
	}
} // namespace daw

