// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_traits.h"

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace daw::impl {
	constexpr bool is_64bit_v = sizeof( size_t ) == sizeof( uint64_t );
} // namespace daw::impl

namespace daw {
#if defined( DAW_FORCE_FNV1A_TYPE )
	using fnv1a_uint_t = DAW_FORCE_FNV1A_TYPE;
#else
	using fnv1a_uint_t =
	  std::conditional_t<impl::is_64bit_v, std::uint64_t, std::uint32_t>;
#endif
} // namespace daw

namespace daw {
	namespace impl {
		[[nodiscard]] constexpr fnv1a_uint_t fnv_prime( ) noexcept {
			return sizeof( fnv1a_uint_t ) == sizeof( std::uint64_t )
			         ? 1099511628211ULL
			         : 16777619UL;
		}

		[[nodiscard]] constexpr fnv1a_uint_t fnv_offset( ) noexcept {
			return sizeof( fnv1a_uint_t ) == sizeof( std::uint64_t )
			         ? 14695981039346656037ULL
			         : 2166136261UL;
		}

	} // namespace impl

	struct fnv1a_hash_t {
		// TODO: check for UB if values are signed
		template<typename Value, std::enable_if_t<std::is_integral_v<Value>,
		                                          std::nullptr_t> = nullptr>
		[[nodiscard]] static constexpr fnv1a_uint_t
		append_hash( fnv1a_uint_t current_hash, Value const &value ) noexcept {
			for( fnv1a_uint_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<fnv1a_uint_t>(
				  ( static_cast<fnv1a_uint_t>( value ) &
				    ( static_cast<fnv1a_uint_t>( 0xFF ) << ( n * 8u ) ) ) >>
				  ( n * 8u ) );
				current_hash *= impl::fnv_prime( );
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2,
		         std::enable_if_t<std::is_integral_v<
		                            typename std::iterator_traits<Iterator1>::type>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( Iterator1 first, Iterator2 const last ) const noexcept {
			auto hash = impl::fnv_offset( );
			while( first != last ) {
				hash = append_hash( hash, *first );
				++first;
			}
			return hash;
		}

		template<typename StringViewLike,
		         std::enable_if_t<traits::is_string_view_like_v<StringViewLike>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( StringViewLike &&sv ) const noexcept {
			auto const *first = std::data( sv );
			auto const *const last =
			  std::data( sv ) + static_cast<std::ptrdiff_t>( std::size( sv ) );
			auto hash = impl::fnv_offset( );
			while( first != last ) {
				hash = append_hash( hash, *first );
			}
			return hash;
		}

		template<
		  typename Member, fnv1a_uint_t N,
		  std::enable_if_t<std::is_integral_v<Member>, std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( Member const ( &member )[N] ) const noexcept {
			return operator( )( member,
			                    std::next( member, static_cast<intmax_t>( N ) ) );
		}

		template<typename Integral, std::enable_if_t<std::is_integral_v<Integral>,
		                                             std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( Integral const value ) const noexcept {
			return append_hash( impl::fnv_offset( ), value );
		}

		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( char const *ptr ) const noexcept {
			auto hash = impl::fnv_offset( );
			while( *ptr != '\0' ) {
				hash = hash ^ static_cast<fnv1a_uint_t>( *ptr );
				hash *= impl::fnv_prime( );
				++ptr;
			}
			return hash;
		}

		template<typename T>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( T const *const ptr ) const noexcept {
			auto hash = impl::fnv_offset( );
			auto bptr = static_cast<uint8_t const *const>( ptr );
			for( fnv1a_uint_t n = 0; n < sizeof( T ); ++n ) {
				hash = hash ^ static_cast<fnv1a_uint_t>( bptr[n] );
				hash *= impl::fnv_prime( );
			}
			return hash;
		}
	};

	template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
	                                            std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr fnv1a_uint_t fnv1a_hash( Integer value ) noexcept {
		return fnv1a_hash_t{ }( value );
	}

	[[nodiscard]] constexpr fnv1a_uint_t fnv1a_hash( char const *ptr ) noexcept {
		auto hash = impl::fnv_offset( );
		while( *ptr != 0 ) {
			hash = fnv1a_hash_t::append_hash( hash, *ptr );
			++ptr;
		}
		return hash;
	}

	template<typename CharT>
	[[nodiscard]] constexpr fnv1a_uint_t fnv1a_hash( CharT const *ptr,
	                                                 std::size_t len ) noexcept {
		auto hash = impl::fnv_offset( );
		auto const last = std::next( ptr, static_cast<std::ptrdiff_t>( len ) );
		while( ptr != last ) {
			hash = fnv1a_hash_t::append_hash( hash, *ptr );
			++ptr;
		}
		return hash;
	}

	template<typename StringViewLike,
	         std::enable_if_t<traits::is_string_view_like_v<StringViewLike>,
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] fnv1a_uint_t fnv1a_hash( StringViewLike &&sv ) {
		return fnv1a_hash( std::data( sv ), std::size( sv ) );
	}

	template<fnv1a_uint_t N>
	[[nodiscard]] constexpr fnv1a_uint_t
	fnv1a_hash( char const ( &ptr )[N] ) noexcept {
		return fnv1a_hash( ptr, N );
	}
} // namespace daw
