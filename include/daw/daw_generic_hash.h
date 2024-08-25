// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "cpp_17.h"
#include "daw_compiler_fixups.h"
#include "daw_move.h"
#include "impl/daw_is_string_view_like.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace daw {
#if defined( DAW_GENERIC_HASH_TYPE )
	using genhash_uint_t = DAW_GENERIC_HASH_TYPE;
#else
	using genhash_uint_t = std::size_t;
#endif

	template<size_t HashBytes>
	struct generic_hash_t;

	template<>
	struct generic_hash_t<2> {
		using hash_value_t = std::uint16_t;
		static constexpr std::size_t const hash_size = 2;
		static constexpr hash_value_t hash_init = 0;

	private:
		template<typename Result, typename T>
		static constexpr Result
		extract_value( T value, std::size_t rshft_bits = 0 ) noexcept {
			static_assert( sizeof( Result ) <= sizeof( T ),
			               "result size must be <= to source size" );
			auto const mask = static_cast<T>( ~( Result{ } & 0 ) ) << rshft_bits;
			value &= mask;
			value >>= rshft_bits;
			return static_cast<Result>( value );
		}

		template<typename Value>
		static constexpr hash_value_t
		append_hash_block( hash_value_t current_hash,
		                   Value value,
		                   std::size_t count = sizeof( Value ) ) noexcept {
			hash_value_t const JODY_HASH_CONSTANT = 0x1F5B;
			hash_value_t const JODY_HASH_SHIFT = 14;

			auto len = count / sizeof( hash_value_t );
			for( ; len > 0; --len ) {
				auto const element =
				  extract_value<hash_value_t>( value, len * sizeof( hash_value_t ) );
				current_hash += element;
				current_hash += JODY_HASH_CONSTANT;
				current_hash =
				  static_cast<hash_value_t>( current_hash << JODY_HASH_SHIFT ) |
				  current_hash >> ( sizeof( hash_value_t ) * 8 - JODY_HASH_SHIFT );
				current_hash ^= element;
				current_hash =
				  static_cast<hash_value_t>( current_hash << JODY_HASH_SHIFT ) |
				  current_hash >> ( sizeof( hash_value_t ) * 8 - JODY_HASH_SHIFT );
				current_hash ^= JODY_HASH_CONSTANT;
				current_hash += element;
			}
			return current_hash;
		}

	public:
		template<
		  typename Value,
		  std::enable_if_t<std::is_integral_v<Value>, std::nullptr_t> = nullptr>
		static constexpr hash_value_t append_hash( hash_value_t current_hash,
		                                           Value const & ) noexcept {
			// TODO	static_assert( sizeof( Value ) >= sizeof( hash_value_t ), "Value
			// must be at least 2 bytes" );
			// TODO
			return current_hash;
		}
	};

	template<>
	struct generic_hash_t<4> {
		using hash_value_t = std::uint32_t;
		static constexpr std::size_t const hash_size = 4;
		static constexpr hash_value_t const hash_init = 2166136261UL;

		template<
		  typename Value,
		  std::enable_if_t<std::is_integral_v<Value>, std::nullptr_t> = nullptr>
		static constexpr hash_value_t append_hash( hash_value_t current_hash,
		                                           Value const &value ) noexcept {
			hash_value_t const fnv1a_prime = 16777619UL;
			for( std::size_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<hash_value_t>(
				  ( static_cast<uintmax_t>( value ) &
				    ( static_cast<uintmax_t>( 0xFF ) << ( n * 8u ) ) ) >>
				  ( n * 8u ) );
				current_hash *= fnv1a_prime;
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2>
		constexpr hash_value_t operator( )( Iterator1 first,
		                                    Iterator2 const last ) const noexcept {
			auto hash = hash_init;
			while( first != last ) {
				hash = append_hash( hash, *first );
				++first;
			}
			return hash;
		}
		template<
		  typename Member,
		  std::size_t N,
		  std::enable_if_t<std::is_integral_v<Member>, std::nullptr_t> = nullptr>
		constexpr hash_value_t
		operator( )( Member const ( &member )[N] ) const noexcept {
			DAW_UNSAFE_BUFFER_FUNC_START
			return operator( )( member, member + static_cast<intmax_t>( N ) );
			DAW_UNSAFE_BUFFER_FUNC_STOP
		}

		template<
		  typename Integral,
		  std::enable_if_t<std::is_integral_v<Integral>, std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Integral const value ) const noexcept {
			return append_hash( hash_init, value );
		}
	};
	template<>
	struct generic_hash_t<8> {
		using hash_value_t = std::uint64_t;
		static constexpr std::size_t const hash_size = 8;
		static constexpr hash_value_t const hash_init = 14695981039346656037ULL;

		template<
		  typename Value,
		  std::enable_if_t<std::is_integral_v<Value>, std::nullptr_t> = nullptr>
		static constexpr hash_value_t append_hash( hash_value_t current_hash,
		                                           Value const &value ) noexcept {
			hash_value_t const fnv1a_prime = 1099511628211ULL;
			for( std::size_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<hash_value_t>(
				  ( static_cast<uintmax_t>( value ) &
				    ( static_cast<uintmax_t>( 0xFF ) << ( n * 8u ) ) ) >>
				  ( n * 8u ) );
				current_hash *= fnv1a_prime;
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2>
		constexpr hash_value_t operator( )( Iterator1 first,
		                                    Iterator2 const last ) const noexcept {
			auto hash = hash_init;
			while( first != last ) {
				hash = append_hash( hash, *first );
				++first;
			}
			return hash;
		}
		template<
		  typename Member,
		  std::size_t N,
		  std::enable_if_t<std::is_integral_v<Member>, std::nullptr_t> = nullptr>
		constexpr hash_value_t
		operator( )( Member const ( &member )[N] ) const noexcept {
			DAW_UNSAFE_BUFFER_FUNC_START
			return operator( )( member, member + static_cast<intmax_t>( N ) );
			DAW_UNSAFE_BUFFER_FUNC_STOP
		}

		template<
		  typename Integral,
		  std::enable_if_t<std::is_integral_v<Integral>, std::nullptr_t> = nullptr>
		constexpr hash_value_t operator( )( Integral const value ) const noexcept {
			return append_hash( hash_init, value );
		}

		template<typename T>
		constexpr hash_value_t operator( )( T const *const ptr ) const noexcept {
			auto hash = hash_init;
			auto bptr = static_cast<uint8_t const *const>( ptr );
			for( std::size_t n = 0; n < sizeof( T ); ++n ) {
				append_hash( hash, bptr[n] );
			}
			return hash;
		}
	};

	template<size_t HashBytes = sizeof( genhash_uint_t ),
	         typename T,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr auto generic_hash( T const value ) noexcept {
		return generic_hash_t<HashBytes>{ }( value );
	}

	template<size_t HashBytes = sizeof( genhash_uint_t ),
	         typename Iterator,
	         typename IteratorL>
	constexpr auto generic_hash( Iterator first, IteratorL const last ) noexcept {
		using hash_t = generic_hash_t<HashBytes>;
		auto hash = hash_t::hash_init;
		while( first != last ) {
			hash = hash_t::append_hash( hash, *first );
			++first;
		}
		return hash;
	}

	template<size_t HashBytes = sizeof( genhash_uint_t ), typename Iterator>
	constexpr auto generic_hash( Iterator first,
	                             std::size_t const len ) noexcept {
		using hash_t = generic_hash_t<HashBytes>;
		auto hash = hash_t::hash_init;
		for( std::size_t n = 0; n < len; ++n ) {
			hash = hash_t::append_hash( hash, *first );
			++first;
		}
		return hash;
	}

	template<size_t HashBytes = sizeof( genhash_uint_t ),
	         typename StringViewLike,
	         std::enable_if_t<traits_is_sv::is_string_view_like_v<StringViewLike>,
	                          std::nullptr_t> = nullptr>
	auto generic_hash( StringViewLike const &&sv ) noexcept {
		return generic_hash<HashBytes>( sv.data( ), sv.size( ) );
	}

	template<size_t HashBytes = sizeof( genhash_uint_t ), std::size_t N>
	constexpr auto generic_hash( char const ( &ptr )[N] ) noexcept {
		return generic_hash<HashBytes>( ptr, N );
	}
} // namespace daw
