// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_algorithm.h"

#include <ciso646>
#include <functional>
#include <optional>
#include <vector>

namespace daw {
	template<typename Key, typename Hash = std::hash<Key>>
	class hash_set_t {
		std::vector<std::optional<Key>> m_indices;

		[[nodiscard]] static constexpr size_t scale_hash( size_t hash,
		                                                  size_t range_size ) {
			size_t const prime_a = 18446744073709551557u;
			size_t const prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % range_size;
		}

		[[nodiscard]] std::optional<size_t> find_index( size_t hash,
		                                                Key const &key ) const {
			size_t const scaled_hash = scale_hash( hash, m_indices.size( ) );

			for( size_t n = scaled_hash; n < m_indices.size( ); ++n ) {
				if( !m_indices[n] ) {
					return n;
				}
				if( *m_indices[n] == key ) {
					return n;
				}
			}
			for( size_t n = 0; n < scaled_hash; ++n ) {
				if( !m_indices[n] ) {
					return n;
				}
				if( *m_indices[n] == key ) {
					return n;
				}
			}
			return { };
		}

	public:
		hash_set_t( size_t range_size ) noexcept
		  : m_indices( range_size, std::nullopt ) {}

		size_t insert( Key const &key ) {
			auto const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( !index ) {
				throw std::out_of_range( "Hash table is full" );
			}
			m_indices[*index] = key;
			return *index;
		}

		std::optional<size_t> erase( Key const &key ) {
			auto const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( !index ) {
				return { };
			}
			m_indices[index] = std::nullopt;
			return index;
		}

		[[nodiscard]] bool exists( Key const &key ) const noexcept {
			auto const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			return static_cast<bool>( index ) and
			       static_cast<bool>( m_indices[*index] );
		}

		[[nodiscard]] bool count( Key const &key ) const noexcept {
			return exists( key ) ? 1 : 0;
		}

		[[nodiscard]] size_t capacity( ) const noexcept {
			return m_indices.size( );
		}

		[[nodiscard]] size_t size( ) const noexcept {
			return daw::algorithm::accumulate(
			  std::begin( m_indices ), std::end( m_indices ), 0ULL,
			  []( auto const &opt ) {
				  return static_cast<bool>( opt ) ? 1ULL : 0ULL;
			  } );
		}
	};
} // namespace daw
