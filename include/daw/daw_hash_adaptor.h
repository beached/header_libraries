// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

namespace daw {
	constexpr size_t scale_hash( size_t hash, size_t range_size ) {
		size_t const prime_a = 18446744073709551557u;
		size_t const prime_b = 18446744073709551533u;
		return ( hash * prime_a + prime_b ) % range_size;
	}

	template<typename Key, typename Hash = std::hash<Key>>
	class hash_adaptor_t {
		std::vector<std::optional<Key>> m_indices;

		size_t find_index( size_t hash, Key const & key ) {
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
			throw std::out_of_range( "Hash table is full" );
		}

	public:
		hash_adaptor_t( size_t range_size ) noexcept
		  : m_indices( range_size, std::nullopt ) {}

		size_t insert( Key const &key ) {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			m_indices[index] = key;
			return index;
		}

		size_t erase( Key const &key ) {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			m_indices[index] = std::nullopt;
			return index;
		}

		bool exists( Key const &key ) noexcept {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			return static_cast<bool>( m_indices[index] ) ? 1 : 0;
		}

		bool count( Key const & key ) noexcept {
			return exists( key ) ? 1 : 0;
		}
	};

	template<typename Key, size_t Capacity, typename Hash = std::hash<Key>>
	class static_hash_adaptor_t {
		std::array<std::optional<size_t>, Capacity> m_indices{};

		constexpr size_t find_index( size_t hash, Key const & key ) const {
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
			std::terminate( );
		}

	public:
		constexpr static_hash_adaptor_t( ) noexcept = default;

		constexpr size_t insert( Key const &key ) noexcept {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			m_indices[index] = key;
			return index;
		}

		constexpr size_t erase( Key const &key ) noexcept {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			m_indices[index] = std::nullopt;
			return index;
		}

		constexpr bool exists( Key const &key ) noexcept {
			auto const hash = Hash{}( key );
			auto const index = find_index( hash, key );
			return static_cast<bool>( m_indices[index] );
		}

		constexpr bool count( Key const & key ) noexcept {
			return exists( key ) ? 1 : 0;
		}
	};
} // namespace daw

