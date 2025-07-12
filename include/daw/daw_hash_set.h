// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_algorithm.h"
#include "daw/daw_attributes.h"
#include "daw/daw_check_exceptions.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

namespace daw {
	template<typename Key, typename Hash = std::hash<Key>>
	class hash_set_t {
		std::vector<std::optional<Key>> m_indices;

		[[nodiscard]] static constexpr std::uint64_t
		scale_hash( std::uint64_t hash, std::uint64_t range_size ) {
			constexpr std::uint64_t prime_a = 18446744073709551557ull;
			constexpr std::uint64_t prime_b = 18446744073709551533ull;
			return ( hash * prime_a + prime_b ) % range_size;
		}

		[[nodiscard]] std::optional<std::uint64_t>
		find_index( std::uint64_t hash, Key const &key ) const {
			std::uint64_t const scaled_hash = scale_hash( hash, m_indices.size( ) );

			for( std::uint64_t n = scaled_hash; n < m_indices.size( ); ++n ) {
				if( not m_indices[n] ) {
					return n;
				}
				if( *m_indices[n] == key ) {
					return n;
				}
			}
			for( std::uint64_t n = 0; n < scaled_hash; ++n ) {
				if( not m_indices[n] ) {
					return n;
				}
				if( *m_indices[n] == key ) {
					return n;
				}
			}
			return { };
		}

	public:
		hash_set_t( std::uint64_t range_size ) noexcept
		  : m_indices( range_size, std::nullopt ) {}

		std::uint64_t insert( Key const &key ) {
			auto const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( not index ) {
				DAW_THROW_OR_TERMINATE( std::out_of_range, "Hash table is full" );
			}
			m_indices[*index] = key;
			return *index;
		}

		std::optional<std::uint64_t> erase( Key const &key ) {
			auto const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( not index ) {
				return { };
			}
			m_indices[index] = std::nullopt;
			return index;
		}

		[[nodiscard]] bool exists( Key const &key ) const noexcept {
			auto const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( index ) {
#if defined( DAW_HAS_MSVC )
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif
				return static_cast<bool>( m_indices[*index] );
#if defined( HAS_HAS_MSVC )
#pragma warning( pop )
#endif
			}
			return false;
		}

		[[nodiscard]] bool count( Key const &key ) const noexcept {
			return exists( key ) ? 1 : 0;
		}

		[[nodiscard]] std::uint64_t capacity( ) const noexcept {
			return m_indices.size( );
		}

		[[nodiscard]] std::uint64_t size( ) const noexcept {
			return daw::algorithm::accumulate(
			  std::begin( m_indices ), std::end( m_indices ), 0ULL,
			  []( auto const &opt ) {
				  return static_cast<bool>( opt ) ? 1ULL : 0ULL;
			  } );
		}
	};
} // namespace daw
