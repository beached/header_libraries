// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
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

#include <limits>

#include "daw_fnv1a_hash.h"

namespace daw {
	template<typename T, size_t Capacity>
	struct static_hash_t final {
		using value_type = std::decay_t<T>;
		using reference = value_type &;
		using const_reference = value_type const &;
		enum class hash_sentinals : size_t { empty, Size };

		struct hash_item {
			size_t hash_value;
			value_type value;
			constexpr hash_item( )
			  : hash_value{}
			  , value{} {}

			constexpr hash_item( size_t h, value_type v )
			  : hash_value{std::move( h )}
			  , value{std::move( v )} {}

		}; // hash_item

		using values_type = hash_item[Capacity];

	private:
		values_type m_values;

		static constexpr size_t scale_hash( size_t hash_value ) {
			const size_t prime_a = 18446744073709551557u;
			const size_t prime_b = 18446744073709551533u;
			return ( hash_value * prime_a + prime_b ) % Capacity;
		}

		template<typename K>
		static constexpr size_t hash_fn( K const &key ) {
			return ( daw::fnv1a_hash( key ) %
			         ( std::numeric_limits<size_t>::max( ) -
			           static_cast<size_t>( hash_sentinals::Size ) ) ) +
			       static_cast<size_t>( hash_sentinals::Size );
		}

		static constexpr size_t hash_fn( char const *const key ) {
			return ( daw::fnv1a_hash( key ) %
			         ( std::numeric_limits<size_t>::max( ) -
			           static_cast<size_t>( hash_sentinals::Size ) ) ) +
			       static_cast<size_t>( hash_sentinals::Size );
		}

		constexpr size_t find_impl( size_t const hash ) const {
			auto const scaled_hash = scale_hash( hash );
			for( size_t n = scaled_hash; n < Capacity; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<size_t>( hash_sentinals::empty ) ) {
					return n;
				} else if( m_values[n].hash_value == hash ) {
					return n;
				}
			}
			for( size_t n = 0; n < scaled_hash; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<size_t>( hash_sentinals::empty ) ) {
					return n;
				} else if( m_values[n].hash_value == hash ) {
					return n;
				}
			}
			return Capacity;
		}

		constexpr size_t find_next_empty( size_t const pos ) const {
			for( size_t n = pos; n < Capacity; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<size_t>( hash_sentinals::empty ) ) {
					return n;
				}
			}
			for( size_t n = 0; n < pos; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<size_t>( hash_sentinals::empty ) ) {
					return n;
				}
			}
			return Capacity;
		}

	public:
		constexpr static_hash_t( )
		  : m_values{} {
			daw::algorithm::fill_n( m_values.begin( ), Capacity, hash_item{} );
		}

		constexpr static_hash_t(
		  std::initializer_list<std::pair<char const *const, value_type>> items ) {
			for( auto const &item : items ) {
				auto const hash = hash_fn( item.first );
				auto const pos = find_impl( hash );
				m_values[pos].hash_value = hash;
				m_values[pos].value = std::move( item.second );
			}
		}

		template<typename K>
		constexpr static_hash_t(
		  std::initializer_list<std::pair<K, value_type>> items ) {
			for( auto const &item : items ) {
				auto const hash = hash_fn( item.first );
				auto const pos = find_impl( hash );
				m_values[pos].hash_value = hash;
				m_values[pos].value = std::move( item.second );
			}
		}

		template<typename K>
		constexpr size_t find( K const &key ) const {
			auto const hash = hash_fn( key );
			return find_impl( hash );
		}

		template<typename K>
		constexpr reference operator[]( K const &key ) {
			auto const hash = hash_fn( key );
			auto const pos = find_impl( hash );
			m_values[pos].hash_value = hash;
			return m_values[pos].value;
		}

		constexpr reference operator[]( char const *const key ) {
			auto const hash = hash_fn( key );
			auto const pos = find_impl( hash );
			m_values[pos].hash_value = hash;
			return m_values[pos].value;
		}

		template<typename K>
		constexpr const_reference operator[]( K const &key ) const {
			auto const hash = hash_fn( key );
			auto const pos = find_impl( hash );
			m_values[pos].hash_value = hash;
			return m_values[pos].value;
		}

		constexpr const_reference operator[]( char const *const key ) const {
			auto hash = hash_fn( key );
			auto const pos = find_impl( hash );
			return m_values[pos].value;
		}

	}; // static_hash_t

} // namespace daw
