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
	template<typename Key>
	struct bounded_hash_set_iterator;

	template<typename Key>
	struct const_bounded_hash_set_iterator {
		using difference_type = std::ptrdiff_t;
		using value_type = Key;
		using pointer = value_type const *;
		using const_pointer = value_type const *;
		using reference = value_type const &;
		using const_reference = value_type const &;
		using iterator_category = std::bidirectional_iterator_tag;

	private:
		std::pair<Key, bool> const *const m_first;
		std::pair<Key, bool> const *const m_last;
		std::pair<Key, bool> const *m_position;

	public:
		constexpr const_bounded_hash_set_iterator(
		  std::pair<Key, bool> const *first, std::pair<Key, bool> const *last,
		  std::pair<Key, bool> const *pos ) noexcept
		  : m_first( first )
		  , m_last( last )
		  , m_position( pos ) {}

		constexpr const_reference operator*( ) const noexcept {
			return m_position->first;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return std::addressof( m_position->first );
		}

		constexpr const_bounded_hash_set_iterator &operator++( ) noexcept {
			++m_position;
			while( !m_position->second and m_position <= m_last ) {
				++m_position;
			}
			return *this;
		}

		constexpr const_bounded_hash_set_iterator operator++( int ) noexcept {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr const_bounded_hash_set_iterator &operator--( ) noexcept {
			--m_position;
			while( !m_position->second and m_position >= m_first ) {
				--m_position;
			}
			return *this;
		}

		constexpr const_bounded_hash_set_iterator operator--( int ) noexcept {
			auto result = *this;
			operator--( );
			return result;
		}

		friend struct bounded_hash_set_iterator<Key>;
		constexpr int compare( const_bounded_hash_set_iterator const & rhs ) const noexcept {
			if( m_position == rhs.m_position ) {
				return 0;
			}
			if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}

		constexpr int compare( bounded_hash_set_iterator<Key> const & rhs ) const noexcept {
			if( m_position == rhs.m_position ) {
				return 0;
			}
			if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}
	};

	template<typename Key>
	struct bounded_hash_set_iterator {
		using difference_type = std::ptrdiff_t;
		using value_type = Key;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using iterator_category = std::bidirectional_iterator_tag;

	private:
		std::pair<Key, bool> const *const m_first;
		std::pair<Key, bool> const *const m_last;
		std::pair<Key, bool> *m_position;

	public:
		constexpr bounded_hash_set_iterator( std::pair<Key, bool> const *first,
		                                     std::pair<Key, bool> const *last,
		                                     std::pair<Key, bool> *pos ) noexcept
		  : m_first( first )
		  , m_last( last )
		  , m_position( pos ) {}

		constexpr reference operator*( ) noexcept {
			return m_position->first;
		}

		constexpr const_reference operator*( ) const noexcept {
			return m_position->first;
		}

		constexpr pointer operator->( ) noexcept {
			return std::addressof( m_position->first );
		}

		constexpr const_pointer operator->( ) const noexcept {
			return std::addressof( m_position->first );
		}

		constexpr bounded_hash_set_iterator &operator++( ) noexcept {
			++m_position;
			while( !m_position->second and m_position <= m_last ) {
				++m_position;
			}
			return *this;
		}

		constexpr bounded_hash_set_iterator operator++( int ) noexcept {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr bounded_hash_set_iterator &operator--( ) noexcept {
			--m_position;
			while( !m_position->second and m_position >= m_first ) {
				--m_position;
			}
			return *this;
		}

		constexpr bounded_hash_set_iterator operator--( int ) noexcept {
			auto result = *this;
			operator--( );
			return result;
		}

		constexpr operator const_bounded_hash_set_iterator<Key>( ) const noexcept {
			return {m_first, m_last, m_position};
		}

		friend struct const_bounded_hash_set_iterator<Key>;
		constexpr int compare( const_bounded_hash_set_iterator<Key> const & rhs ) const noexcept {
			if( m_position == rhs.m_position ) {
				return 0;
			}
				if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}

		constexpr int compare( bounded_hash_set_iterator const & rhs ) const noexcept {
			if( m_position == rhs.m_position ) {
				return 0;
			}
			if( m_position < rhs.m_position ) {
				return -1;
			}
			return 1;
		}
	};

	template<typename Key>
	constexpr bool operator==( bounded_hash_set_iterator<Key> const & lhs, bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key>
	constexpr bool operator==( const_bounded_hash_set_iterator<Key> const & lhs, const_bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key>
	constexpr bool operator==( bounded_hash_set_iterator<Key> const & lhs, const_bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key>
	constexpr bool operator==( const_bounded_hash_set_iterator<Key> const & lhs, bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Key>
	constexpr bool operator!=( bounded_hash_set_iterator<Key> const & lhs, bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key>
	constexpr bool operator!=( const_bounded_hash_set_iterator<Key> const & lhs, const_bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key>
	constexpr bool operator!=( bounded_hash_set_iterator<Key> const & lhs, const_bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key>
	constexpr bool operator!=( const_bounded_hash_set_iterator<Key> const & lhs, bounded_hash_set_iterator<Key> const & rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Key, size_t Capacity, typename Hash = std::hash<Key>,
	         typename KeyEqual = std::equal_to<Key>>
	struct bounded_hash_set_t {
		using key_type = Key;
		using value_type = Key;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using hasher = Hash;
		using key_equal = KeyEqual;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using node_type = std::pair<key_type, bool>;
		using iterator = bounded_hash_set_iterator<key_type>;
		using const_iterator = const_bounded_hash_set_iterator<key_type>;

	private:
		std::array<node_type, Capacity> m_data{};

		static constexpr size_type scale_hash( size_type hash,
		                                       size_type range_size ) noexcept {
			size_type const prime_a = 18446744073709551557u;
			size_type const prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % range_size;
		}

		constexpr std::optional<size_type> find_index( size_type hash,
		                                               Key const &key ) const
		  noexcept {
			size_type const scaled_hash = scale_hash( hash, m_data.size( ) );

			for( size_type n = scaled_hash; n < m_data.size( ); ++n ) {
				if( !m_data[n].second ) {
					return n;
				}
				if( key_equal{}( m_data[n].first, key ) ) {
					return n;
				}
			}
			for( size_type n = 0; n < scaled_hash; ++n ) {
				if( !m_data[n].second ) {
					return n;
				}
				if( key_equal{}( m_data[n].first, key ) ) {
					return n;
				}
			}
			return {};
		}

	public:
		constexpr bounded_hash_set_t( ) noexcept(
		  std::is_nothrow_default_constructible_v<Key> ) = default;

		constexpr iterator insert( Key const &key ) noexcept {
			auto const index = find_index( hasher{}( key ), key );
			if( !index ) {
				// Full
				std::terminate( );
			}
			if( !m_data[*index].second ) {
				m_data[*index].first = key;
				m_data[*index].second = true;
			}
			return {m_data.data( ), m_data.data( ) + m_data.size( ),
			        m_data.data( ) + *index};
		}

		constexpr iterator insert( Key &&key ) noexcept {
			auto const index = find_index( key );
			if( !index ) {
				// Full
				std::terminate( );
			}
			if( !m_data[*index].second ) {
				m_data[*index].first = std::move( key );
				m_data[*index].second = true;
			}
			return {m_data.data( ), m_data.data( ) + m_data.size( ),
			        m_data.data( ) + *index};
		}

		constexpr std::optional<size_type> erase( Key const &key ) noexcept {
			auto const hash = hasher{}( key );
			auto const index = find_index( hash, key );
			if( !index ) {
				return {};
			}
			// m_data[*index].first = Key{};
			m_data[*index].second = false;
			return *index;
		}

		constexpr bool exists( Key const &key ) const noexcept {
			auto const hash = hasher{}( key );
			auto const index = find_index( hash, key );
			return static_cast<bool>( index ) and m_data[*index].second;
		}

		constexpr bool count( Key const &key ) const noexcept {
			return exists( key ) ? 1 : 0;
		}

		static constexpr size_type capacity( ) noexcept {
			return Capacity;
		}

		constexpr size_type size( ) const noexcept {
			return daw::algorithm::accumulate(
			  std::begin( m_data ), std::end( m_data ), 0ULL,
			  []( auto const &opt ) { return opt.second ? 1ULL : 0ULL; } );
		}

		constexpr size_type empty( ) const noexcept {
			return daw::algorithm::find_if( std::cbegin( m_data ), std::cend( m_data ), []( auto const & value ) {
				return value.second;
			}) == std::cend( m_data );
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
			auto idx = find_index( hasher{}( key ), key );
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
} // namespace daw
