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
#include "daw/daw_arith_traits.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_remove_cvref.h"

#include <limits>

namespace daw {
	template<typename T, std::size_t Capacity>
	struct static_hash_t {
		using value_type = daw::remove_cvref_t<T>;
		using reference = value_type &;
		using const_reference = value_type const &;
		enum class hash_sentinals : std::size_t { empty, Size };

		struct hash_item {
			std::uint64_t hash_value = 0;
			value_type value = value_type{ };
			hash_item( ) = default;

			constexpr hash_item( std::uint64_t h, value_type const &v )
			  : hash_value{ h }
			  , value{ v } {}

			constexpr hash_item( std::uint64_t h, value_type &&v )
			  : hash_value{ h }
			  , value{ std::move( v ) } {}

		}; // hash_item

		using values_type = hash_item[Capacity];

	private:
		values_type m_values;

		[[nodiscard]] static constexpr std::size_t
		scale_hash( std::uint64_t hash_value ) {
			constexpr auto prime_a = 18446744073709551557ULL;
			constexpr auto prime_b = 18446744073709551533ULL;
			return static_cast<std::size_t>( ( hash_value * prime_a + prime_b ) %
			                                 Capacity );
		}

		template<typename K>
		[[nodiscard]] static constexpr std::size_t hash_fn( K const &key ) {
			return ( daw::fnv1a_hash( key ) %
			         ( max_value<std::size_t> -
			           static_cast<std::size_t>( hash_sentinals::Size ) ) ) +
			       static_cast<std::size_t>( hash_sentinals::Size );
		}

		[[nodiscard]] static constexpr std::size_t
		hash_fn( char const *const key ) {
			return ( daw::fnv1a_hash( key ) %
			         ( max_value<std::size_t> -
			           static_cast<std::size_t>( hash_sentinals::Size ) ) ) +
			       static_cast<std::size_t>( hash_sentinals::Size );
		}

		[[nodiscard]] constexpr std::size_t find_impl( std::uint64_t hash ) const {
			auto const scaled_hash = scale_hash( hash );
			for( std::size_t n = scaled_hash; n < Capacity; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<std::size_t>( hash_sentinals::empty ) ) {
					return n;
				} else if( m_values[n].hash_value == hash ) {
					return n;
				}
			}
			for( std::size_t n = 0; n < scaled_hash; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<std::size_t>( hash_sentinals::empty ) ) {
					return n;
				} else if( m_values[n].hash_value == hash ) {
					return n;
				}
			}
			return Capacity;
		}

		[[nodiscard]] constexpr std::size_t
		find_next_empty( std::size_t const pos ) const {
			for( std::size_t n = pos; n < Capacity; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<std::size_t>( hash_sentinals::empty ) ) {
					return n;
				}
			}
			for( std::size_t n = 0; n < pos; ++n ) {
				if( m_values[n].hash_value ==
				    static_cast<std::size_t>( hash_sentinals::empty ) ) {
					return n;
				}
			}
			return Capacity;
		}

	public:
		constexpr static_hash_t( )
		  : m_values{ } {
			daw::algorithm::fill_n( m_values.begin( ), Capacity, hash_item{ } );
		}

		constexpr static_hash_t(
		  std::initializer_list<std::pair<char const *const, value_type>> items ) {
			for( auto const &item : items ) {
				auto const hash = hash_fn( item.first );
				auto const pos = find_impl( hash );
				m_values[pos].hash_value = hash;
				m_values[pos].value = item.second;
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
		[[nodiscard]] constexpr std::size_t find( K const &key ) const {
			auto const hash = hash_fn( key );
			return find_impl( hash );
		}

		template<typename K>
		[[nodiscard]] constexpr reference operator[]( K const &key ) {
			auto const hash = hash_fn( key );
			auto const pos = find_impl( hash );
			m_values[pos].hash_value = hash;
			return m_values[pos].value;
		}

		[[nodiscard]] constexpr reference operator[]( char const *const key ) {
			auto const hash = hash_fn( key );
			auto const pos = find_impl( hash );
			m_values[pos].hash_value = hash;
			return m_values[pos].value;
		}

		template<typename K>
		[[nodiscard]] constexpr const_reference operator[]( K const &key ) const {
			auto const hash = hash_fn( key );
			auto const pos = find_impl( hash );
			m_values[pos].hash_value = hash;
			return m_values[pos].value;
		}

		[[nodiscard]] constexpr const_reference
		operator[]( char const *const key ) const {
			auto hash = hash_fn( key );
			auto const pos = find_impl( hash );
			return m_values[pos].value;
		}
	}; // static_hash_t
} // namespace daw
