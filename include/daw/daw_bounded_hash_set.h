// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <functional>
#include <optional>
#include <vector>

#include "daw_algorithm.h"

namespace daw {
	template<typename Key, size_t Capacity, typename Hash = std::hash<Key>>
	class bounded_hash_set_t {
		std::array<std::pair<Key, bool>, Capacity> m_indices{};

		static constexpr size_t scale_hash( size_t hash,
		                                    size_t range_size ) noexcept {
			size_t const prime_a = 18446744073709551557u;
			size_t const prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % range_size;
		}

		constexpr std::optional<size_t> find_index( size_t hash,
		                                            Key const &key ) const
		  noexcept {
			size_t const scaled_hash = scale_hash( hash, m_indices.size( ) );

			for( size_t n = scaled_hash; n < m_indices.size( ); ++n ) {
				if( !m_indices[n].second ) {
					return n;
				}
				if( m_indices[n].first == key ) {
					return n;
				}
			}
			for( size_t n = 0; n < scaled_hash; ++n ) {
				if( !m_indices[n].second ) {
					return n;
				}
				if( m_indices[n].first == key ) {
					return n;
				}
			}
			return {};
		}

	public:
		constexpr bounded_hash_set_t( ) noexcept(
		  std::is_nothrow_default_constructible_v<Key> ) = default;

		constexpr std::optional<size_t> find( Key const &key ) const noexcept {
			auto const hash = Hash{}( key );
			return find_index( hash, key );
		}

		constexpr size_t insert( Key const &key ) noexcept {
			auto const index = find( key );
			if( !index ) {
				// Full
				std::terminate( );
			}
			if( !m_indices[*index].second ) {
				m_indices[*index].first = key;
				m_indices[*index].second = true;
			}
			return *index;
		}

		constexpr size_t insert( Key &&key ) noexcept {
			auto const index = find( key );
			if( !index ) {
				// Full
				std::terminate( );
			}
			if( !m_indices[*index].second ) {
				m_indices[*index].first = std::move( key );
				m_indices[*index].second = true;
			}
			return *index;
		}

		constexpr std::optional<size_t> erase( Key const &key ) noexcept {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			if( !index ) {
				return {};
			}
			// m_indices[*index].first = Key{};
			m_indices[*index].second = false;
			return *index;
		}

		constexpr bool exists( Key const &key ) const noexcept {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			return static_cast<bool>( index ) and m_indices[*index].second;
		}

		constexpr bool count( Key const &key ) const noexcept {
			return exists( key ) ? 1 : 0;
		}

		static constexpr size_t capacity( ) noexcept {
			return Capacity;
		}

		constexpr size_t size( ) const noexcept {
			return daw::algorithm::accumulate(
			  std::begin( m_indices ), std::end( m_indices ), 0ULL,
			  []( auto const &opt ) { return opt.second ? 1ULL : 0ULL; } );
		}
	};
} // namespace daw
