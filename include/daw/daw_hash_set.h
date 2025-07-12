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

#include <cassert>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

namespace daw {
	template<typename Key, typename Hash = std::hash<Key>>
	class hash_set_t {
		std::vector<std::optional<Key>> m_indices;

		[[nodiscard]] static constexpr std::size_t
		scale_hash( std::uint64_t hash, std::size_t range_size ) {
			constexpr std::uint64_t prime_a = 18446744073709551557ull;
			constexpr std::uint64_t prime_b = 18446744073709551533ull;
			return static_cast<std::size_t>( hash * prime_a + prime_b ) % range_size;
		}

		[[nodiscard]] std::optional<std::size_t>
		find_index( std::uint64_t hash, Key const &key ) const {
			std::size_t const scaled_hash = scale_hash( hash, m_indices.size( ) );

			for( std::size_t n = scaled_hash; n < m_indices.size( ); ++n ) {
				if( not m_indices[n] ) {
					return n;
				}
				if( *m_indices[n] == key ) {
					return n;
				}
			}
			for( std::size_t n = 0; n < scaled_hash; ++n ) {
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
		hash_set_t( std::size_t range_size ) noexcept
		  : m_indices( range_size, std::nullopt ) {}

		std::uint64_t insert( Key const &key ) {
			std::uint64_t const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( not index ) {
				DAW_THROW_OR_TERMINATE( std::out_of_range, "Hash table is full" );
			}
			m_indices[*index] = key;
			return *index;
		}

		std::optional<std::size_t> erase( Key const &key ) {
			std::uint64_t const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( not index ) {
				return { };
			}
			m_indices[index] = std::nullopt;
			return index;
		}

		[[nodiscard]] bool exists( Key const &key ) const noexcept {
			std::uint64_t const hash = Hash{ }( key );
			auto const index = find_index( hash, key );
			if( index ) {
				return m_indices[*index].has_value( );
			}
			return false;
		}

		[[nodiscard]] std::size_t count( Key const &key ) const noexcept {
			return exists( key ) ? 1U : 0U;
		}

		[[nodiscard]] std::size_t capacity( ) const noexcept {
			return m_indices.size( );
		}

		[[nodiscard]] std::size_t size( ) const noexcept {
			return daw::algorithm::accumulate( std::begin( m_indices ),
			                                   std::end( m_indices ), std::size_t{ },
			                                   []( auto const &opt ) {
				                                   return opt.has_value( ) ? 1U : 0U;
			                                   } );
		}
	};
} // namespace daw
