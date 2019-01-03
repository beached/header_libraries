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

#include <optional>
#include <memory>
#include <utility>

#include "cpp_17.h"
#include "daw_hash_adaptor.h"
#include "daw_traits.h"
#include "iterator/daw_zipiter.h"

namespace daw {
	template<typename Key, typename Value>
	struct key_value_t {
		Key key{};
		Value value{};
		constexpr key_value_t( ) noexcept(
		  std::is_nothrow_default_constructible_v<Key>
		    and std::is_nothrow_default_constructible_v<Value> ) = default;

		template<typename K, typename V>
		constexpr key_value_t( K &&k, V &&v ) noexcept(
		  std::is_nothrow_constructible_v<Key, K>
		    and std::is_nothrow_constructible_v<Value, V> )
		  : key( std::forward<K>( k ) )
		  , value( std::forward<V>( v ) ) {}
	};

	template<typename Key, typename Value>
	struct static_hash_map_item_t {
		bool has_value = false;
		key_value_t<Key, Value> value{};

		constexpr static_hash_map_item_t( ) noexcept(
		  std::is_nothrow_default_constructible_v<key_value_t<Key, Value>> ) =
		  default;

		template<typename K, typename V>
		constexpr static_hash_map_item_t( K &&k, V &&v ) noexcept(
		  std::is_nothrow_constructible_v<key_value_t<Key, Value>, Key, Value> )
		  : has_value( true )
		  , value( std::forward<K>( k ), std::forward<V>( v ) ) {}

		explicit constexpr operator bool( ) const noexcept {
			return has_value;
		}
	};

	template<typename Key, typename Value>
	struct const_static_hash_map_iterator {
		using difference_type = std::ptrdiff_t;
		using value_type = key_value_t<Key, Value>;
		using const_pointer = value_type const *;
		using const_reference = value_type const &;
		using iterator_category = std::bidirectional_iterator_tag;

	private:
		static_hash_map_item_t<Key, Value> const *const m_first;
		static_hash_map_item_t<Key, Value> const *const m_last;
		static_hash_map_item_t<Key, Value> const *m_position;

	public:
		constexpr const_static_hash_map_iterator(
		  static_hash_map_item_t<Key, Value> const *first,
		  static_hash_map_item_t<Key, Value> const *last,
		  static_hash_map_item_t<Key, Value> const *pos ) noexcept
		  : m_first( first )
		  , m_last( last )
		  , m_position( pos ) {}

		constexpr const_reference operator*( ) const noexcept {
			return m_position->value;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return std::addressof( m_position->value );
		}

		constexpr const_static_hash_map_iterator &operator++( ) noexcept {
			++m_position;
			while( !m_position and m_position <= m_last ) {
				++m_position;
			}
			return *this;
		}

		constexpr const_static_hash_map_iterator operator++( int ) noexcept {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr const_static_hash_map_iterator &operator--( ) noexcept {
			--m_position;
			while( !m_position and m_position >= m_first ) {
				--m_position;
			}
			return *this;
		}

		constexpr const_static_hash_map_iterator operator--( int ) noexcept {
			auto result = *this;
			operator--( );
			return result;
		}
	};

	template<typename Key, typename Value>
	struct static_hash_map_iterator {
		using difference_type = std::ptrdiff_t;
		using value_type = key_value_t<Key, Value>;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator_category = std::bidirectional_iterator_tag;

	private:
		static_hash_map_item_t<Key, Value> const *const m_first;
		static_hash_map_item_t<Key, Value> const *const m_last;
		static_hash_map_item_t<Key, Value> *m_position;

	public:
		constexpr static_hash_map_iterator(
		  static_hash_map_item_t<Key, Value> const *first,
		  static_hash_map_item_t<Key, Value> const *last,
		  static_hash_map_item_t<Key, Value> *pos ) noexcept
		  : m_first( first )
		  , m_last( last )
		  , m_position( pos ) {}

		constexpr reference operator*( ) noexcept {
			return m_position->value;
		}

		constexpr const_reference operator*( ) const noexcept {
			return m_position->value;
		}

		constexpr pointer operator->( ) noexcept {
			return std::addressof( m_position->value );
		}

		constexpr const_pointer operator->( ) const noexcept {
			return std::addressof( m_position->value );
		}

		constexpr static_hash_map_iterator &operator++( ) noexcept {
			++m_position;
			while( !m_position and m_position <= m_last ) {
				++m_position;
			}
			return *this;
		}

		constexpr static_hash_map_iterator operator++( int ) noexcept {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr static_hash_map_iterator &operator--( ) noexcept {
			--m_position;
			while( !m_position and m_position >= m_first ) {
				--m_position;
			}
			return *this;
		}

		constexpr static_hash_map_iterator operator--( int ) noexcept {
			auto result = *this;
			operator--( );
			return result;
		}

		constexpr operator const_static_hash_map_iterator<Key, Value>( ) const noexcept {
			return {m_first, m_last, m_position};
		}
	};

	template<typename Key, typename Value, size_t N,
	         typename Hash = std::hash<Key>,
	         typename KeyEqual = std::equal_to<Key>>
	struct static_hash_map {
		using key_type = Key;
		using mapped_type = Value;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using hasher = Hash;
		using key_equal = KeyEqual;
		using value_type = key_value_t<Key, Value>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using iterator = static_hash_map_iterator<Key, Value>;
		using const_iterator = const_static_hash_map_iterator<Key, Value>;

	private:
		std::array<static_hash_map_item_t<Key, Value>, N> m_data{};

		constexpr std::optional<size_type> find_index( Key const &key ) const
		  noexcept {
			size_type const hash = Hash{}( key );
			size_type const scaled_hash = scale_hash( hash, m_data.size( ) );

			for( size_type n = scaled_hash; n < m_data.size( ); ++n ) {
				if( !m_data[n] or key_equal{}( m_data[n].value.key, key ) ) {
					return n;
				}
			}
			for( size_type n = 0; n < scaled_hash; ++n ) {
				if( !m_data[n] or key_equal{}( m_data[n].value.key, key ) ) {
					return n;
				}
			}
			return {};
		}

	public:
		constexpr static_hash_map( ) noexcept(
		  std::is_nothrow_default_constructible_v<
		    static_hash_map_item_t<Key, Value>> ) = default;

		template<size_type ItemCount>
		constexpr static_hash_map(
		  std::pair<Key, Value> const ( &init_values )
		    [ItemCount] ) noexcept( std::
		                              is_nothrow_constructible_v<
		                                static_hash_map_item_t<Key, Value>, Key,
		                                Value> ) {

			static_assert( ItemCount <= N );
			for( auto const &kv : init_values ) {
				insert( kv.first, kv.second );
			}
		}

		template<typename K, typename V,
		         std::enable_if_t<
		           daw::all_true_v<std::is_same_v<Key, daw::remove_cvref_t<K>>,
		                           is_same_v<Value, daw::remove_cvref_t<V>>>,
		           std::nullptr_t> = nullptr>
		constexpr void insert( K &&key, V &&value ) noexcept {
			decltype( auto ) item = m_data[*find_index( key )];
			if( !item ) {
				item.has_value = true;
				item.value.key = std::forward<K>( key );
			}
			item.value.value = std::forward<V>( value );
		}

		constexpr bool exists( Key const &key ) const noexcept {
			auto idx = find_index( key );
			return !idx or m_data[*idx];
		}

		template<typename K>
		constexpr mapped_type &operator[]( K &&key ) noexcept {
			decltype( auto ) item = m_data[*find_index( key )];
			if( !item ) {
				item.has_value = true;
				item.value.key = std::forward<K>( key );
				item.value.value = Value{};
			};
			return item.value;
		}

		constexpr std::optional<mapped_type> try_get( Key const &key ) const
		  noexcept {
			auto idx = find_index( key );
			if( !idx ) {
				return {};
			}
			decltype( auto ) item = m_data[*idx];
			if( !item ) {
				return {};
			}
			return item.value.value;
		}

		constexpr size_type count( Key const &key ) const noexcept {
			if( exists( key ) ) {
				return 1U;
			}
			return 0U;
		}

		static constexpr size_type capacity( ) noexcept {
			return N;
		}

		constexpr size_type size( ) const noexcept {
			return std::accumulate(
			  cbegin( m_data ), cend( m_data ), 0ULL, []( auto &&init, auto &&v ) {
				  if( v ) {
					  return std::forward<decltype( init )>( init ) + 1ULL;
				  }
				  return std::forward<decltype( init )>( init );
			  } );
		}

		constexpr iterator begin( ) noexcept {
			return {m_data.data( ), m_data.data( ) + m_data.size( ), m_data.data( )};
		}

		constexpr const_iterator begin( ) const noexcept {
			return {m_data.data( ), m_data.data( ) + m_data.size( ), m_data.data( )};
		}

		constexpr const_iterator cbegin( ) const noexcept {
			return {m_data.data( ), m_data.data( ) + m_data.size( ), m_data.data( )};
		}

		constexpr iterator end( ) noexcept {
			return {m_data.data( ), m_data.data( ) + m_data.size( ),
			        m_data.data( ) + m_data.size( )};
		}

		constexpr const_iterator end( ) const noexcept {
			return {m_data.data( ), m_data.data( ) + m_data.size( ),
			        m_data.data( ) + m_data.size( )};
		}

		constexpr const_iterator cend( ) const noexcept {
			return {m_data.data( ), m_data.data( ) + m_data.size( ),
			        m_data.data( ) + m_data.size( )};
		}

		constexpr iterator find( Key const &key ) noexcept {
			auto idx = find_index( key );
			if( !idx ) {
				return end( );
			}
			return {m_data.data( ), m_data.data( ) + m_data.size( ), &m_data[*idx]};
		}

		constexpr const_iterator find( Key const &key ) const noexcept {
			auto idx = find_index( key );
			if( !idx ) {
				return end( );
			}
			return {m_data.data( ), m_data.data( ) + m_data.size( ), &m_data[*idx]};
		}
	};

	template<typename Key, typename Value, typename Hash = std::hash<Key>,
	         size_t N>
	constexpr auto
	make_static_hash_map( std::pair<Key, Value> const ( &items )[N] ) noexcept {
		return static_hash_map<Key, Value, N, Hash>( items );
	}
} // namespace daw
