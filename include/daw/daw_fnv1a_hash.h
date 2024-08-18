// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_attributes.h"
#include "daw_bit_cast.h"
#include "daw_compiler_fixups.h"
#include "daw_data_end.h"
#include "impl/daw_is_string_view_like.h"
#include "traits/daw_traits_conditional.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <daw/stdinc/data_access.h>
#include <daw/stdinc/enable_if.h>

namespace daw {
	namespace fnv1a_impl {
		inline constexpr bool is_64bit_v =
		  sizeof( size_t ) == sizeof( std::uint64_t );

	} // namespace fnv1a_impl
#if defined( DAW_FORCE_FNV1A_TYPE )
	using fnv1a_uint_t = DAW_FORCE_FNV1A_TYPE;
#else
	using fnv1a_uint_t =
	  conditional_t<fnv1a_impl::is_64bit_v, std::uint64_t, std::uint32_t>;
#endif
} // namespace daw

namespace daw {
	namespace fnv1a_impl {
		inline constexpr fnv1a_uint_t fnv_prime =
		  sizeof( fnv1a_uint_t ) == sizeof( std::uint64_t ) ? 1099511628211ULL
		                                                    : 16777619UL;

		inline constexpr fnv1a_uint_t fnv_offset =
		  sizeof( fnv1a_uint_t ) == sizeof( std::uint64_t )
		    ? 14695981039346656037ULL
		    : 2166136261UL;

		template<std::size_t N>
		struct byte_array {
			unsigned char values[N];

			constexpr unsigned char const *begin( ) const {
				return values;
			}

			constexpr unsigned char const *end( ) const {
				return values + N;
			}
		};
	} // namespace fnv1a_impl

	struct fnv1a_hash_t {
		// TODO: check for UB if values are signed
		template<
		  typename Value,
		  std::enable_if_t<std::is_integral_v<Value>, std::nullptr_t> = nullptr>
		[[nodiscard]] static constexpr fnv1a_uint_t
		append_hash( fnv1a_uint_t current_hash, Value const &value ) noexcept {
			for( fnv1a_uint_t n = 0; n < sizeof( Value ); ++n ) {
				current_hash ^= static_cast<fnv1a_uint_t>(
				  ( static_cast<fnv1a_uint_t>( value ) &
				    ( fnv1a_uint_t{ 0xFFU } << ( n * 8U ) ) ) >>
				  ( n * 8U ) );
				current_hash *= fnv1a_impl::fnv_prime;
			}
			return current_hash;
		}

		template<typename Iterator1, typename Iterator2>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( Iterator1 first, Iterator2 const last ) const noexcept {
			auto hash = fnv1a_impl::fnv_offset;
			while( first != last ) {
				hash = append_hash( hash, *first );
				++first;
			}
			return hash;
		}

		template<
		  typename StringViewLike,
		  std::enable_if_t<traits_is_sv::is_string_view_like_v<StringViewLike>,
		                   std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( StringViewLike &&sv ) const noexcept {
			auto const *first = std::data( sv );
			auto const *const last = daw::data_end( sv );
			auto hash = fnv1a_impl::fnv_offset;
			while( first != last ) {
				hash = append_hash( hash, *first );
				++first;
			}
			return hash;
		}

		template<
		  typename Member,
		  fnv1a_uint_t N,
		  std::enable_if_t<std::is_integral_v<Member>, std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( Member const ( &member )[N] ) const noexcept {
			DAW_UNSAFE_BUFFER_FUNC_START
			return operator( )( member, member + static_cast<std::ptrdiff_t>( N ) );
			DAW_UNSAFE_BUFFER_FUNC_STOP
		}

		template<
		  typename Integral,
		  std::enable_if_t<std::is_integral_v<Integral>, std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( Integral const value ) const noexcept {
			return append_hash( fnv1a_impl::fnv_offset, value );
		}

		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( char const *ptr ) const noexcept {
			auto hash = fnv1a_impl::fnv_offset;
			while( *ptr != '\0' ) {
				hash = hash ^ static_cast<fnv1a_uint_t>( *ptr );
				hash *= fnv1a_impl::fnv_prime;
				DAW_UNSAFE_BUFFER_FUNC_START
				++ptr;
				DAW_UNSAFE_BUFFER_FUNC_STOP
			}
			return hash;
		}

		template<typename T>
		[[nodiscard]] constexpr fnv1a_uint_t
		operator( )( T const *const ptr ) const noexcept {
			auto hash = fnv1a_impl::fnv_offset;
			assert( ptr );
			auto bytes = daw::bit_cast<fnv1a_impl::byte_array<sizeof( T )>>( *ptr );
			for( auto c : bytes ) {
				hash = hash ^ static_cast<fnv1a_uint_t>( c );
				hash *= fnv1a_impl::fnv_prime;
			}
			return hash;
		}
	};

	template<
	  typename Integer,
	  std::enable_if_t<std::is_integral_v<Integer>, std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr fnv1a_uint_t fnv1a_hash( Integer value ) noexcept {
		return fnv1a_hash_t{ }( value );
	}

	[[nodiscard]] constexpr fnv1a_uint_t fnv1a_hash( char const *ptr ) noexcept {
		auto hash = fnv1a_impl::fnv_offset;
		while( *ptr != 0 ) {
			hash = fnv1a_hash_t::append_hash( hash, *ptr );
			DAW_UNSAFE_BUFFER_FUNC_START
			++ptr;
			DAW_UNSAFE_BUFFER_FUNC_STOP
		}
		return hash;
	}

	template<typename CharT>
	[[nodiscard]] constexpr fnv1a_uint_t fnv1a_hash( CharT const *ptr,
	                                                 std::size_t len ) noexcept {
		auto hash = fnv1a_impl::fnv_offset;
		DAW_UNSAFE_BUFFER_FUNC_START
		auto const last = ptr + static_cast<std::ptrdiff_t>( len );
		while( ptr != last ) {
			hash = fnv1a_hash_t::append_hash( hash, *ptr );
			++ptr;
			DAW_UNSAFE_BUFFER_FUNC_STOP
		}
		return hash;
	}

	template<typename StringViewLike,
	         std::enable_if_t<traits_is_sv::is_string_view_like_v<StringViewLike>,
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] fnv1a_uint_t fnv1a_hash( StringViewLike &&sv ) {
		using namespace std;
		return fnv1a_hash( data( sv ), size( sv ) );
	}

	template<fnv1a_uint_t N>
	[[nodiscard]] constexpr fnv1a_uint_t
	fnv1a_hash( char const ( &ptr )[N] ) noexcept {
		return fnv1a_hash( ptr, N );
	}
} // namespace daw
