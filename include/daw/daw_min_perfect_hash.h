// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include "daw/daw_bounded_vector.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw_algorithm.h"
#include "daw_sort_n.h"

#include <utility>

namespace daw {
	namespace min_perf_hash_impl {
		constexpr size_t get_bucket_count( size_t sz ) {
			size_t ret = 1U;
			do {
				ret = ret << 1U;
				sz = sz >> 1U;
			} while( sz != 0 );
			return ret >> 1U;
		}

		constexpr bool is_64bit_v = sizeof( size_t ) == sizeof( uint64_t );

		[[nodiscard]] constexpr size_t fnv_prime( ) noexcept {
			return is_64bit_v ? 1099511628211ULL : 16777619UL;
		}

		template<size_t N>
		using matching_signed_t = std::conditional_t<
		  N == 64, int64_t,
		  std::conditional_t<
		    N == 32, int32_t,
		    std::conditional_t<N == 16, int16_t,
		                       std::conditional_t<N == 8, int8_t, intmax_t>>>>;
	} // namespace min_perf_hash_impl

	// *****************************************************
	// Forward Declare for Iterator
	template<size_t N, typename Key, typename Value, typename Hasher,
	         typename KeyEqual>
	class perfect_hash_table_iterator;

	template<size_t N, typename Key, typename Value, typename Hasher,
	         typename KeyEqual>
	class const_perfect_hash_table_iterator;

	//*********************************************************************
	// Minimal Perfect Hash Table
	// Adapted from
	// https://blog.demofox.org/2015/12/14/o1-data-lookups-with-minimal-perfect-hashing/
	// and https://blog.gopheracademy.com/advent-2017/mphf/
	template<size_t N, typename Key, typename Value,
	         typename Hasher = std::hash<Key>,
	         typename KeyEqual = std::equal_to<Key>>
	struct perfect_hash_table {
		static_assert( std::is_default_constructible_v<Key> );
		static_assert( std::is_default_constructible_v<Value> );
		static_assert( std::is_trivially_copyable_v<Key> );
		static_assert( std::is_trivially_copyable_v<Value> );
		using key_type = Key;
		using mapped_type = Value;
		using value_type = std::pair<key_type, mapped_type>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using key_equal = KeyEqual;
		using reference = value_type &;
		using const_refernce = value_type const &;
		using pointer = value_type *;
		using hash_result = daw::remove_cvref_t<std::invoke_result_t<Hasher, Key>>;
		using salt_type =
		  min_perf_hash_impl::matching_signed_t<sizeof( hash_result )>;
		using iterator =
		  perfect_hash_table_iterator<N, Key, Value, Hasher, KeyEqual>;
		using const_iterator =
		  const_perfect_hash_table_iterator<N, Key, Value, Hasher, KeyEqual>;

		friend ::daw::perfect_hash_table_iterator<N, Key, Value, Hasher, KeyEqual>;
		friend ::daw::const_perfect_hash_table_iterator<N, Key, Value, Hasher,
		                                                KeyEqual>;

	private:
		struct bucket_t {
			size_type bucket_index = 0;
			daw::bounded_vector_t<std::pair<Key, Value> const *, N> items{};

			constexpr bool operator>( bucket_t const &rhs ) const {
				return bucket_index > bucket_index;
			}
		};

		static constexpr size_type m_num_buckets =
		  min_perf_hash_impl::get_bucket_count( N );
		static constexpr size_type m_bucket_mask = m_num_buckets - 1;

		using values_t = std::array<value_type, N>;
		//***************
		std::array<salt_type, m_num_buckets> m_salts{};
		values_t m_data{};
		//***************

		static constexpr hash_result call_hash( Key const &key ) {
			return Hasher{}( key );
		}

		static constexpr hash_result call_hash( Key const &key, salt_type seed ) {
			return static_cast<hash_result>( seed ) ^ Hasher {}( key );
		}

		static constexpr hash_result first_hash( Key const &key ) {
			return call_hash( key ) & m_bucket_mask;
		}

		constexpr void find_salt_for_bucket( bucket_t const &bucket,
		                                     std::array<bool, N> &slots_claimed ) {
			if( bucket.items.size( ) == 1 ) {
				for( size_type i = 0; i < slots_claimed.size( ); ++i ) {
					if( not slots_claimed[i] ) {
						slots_claimed[i] = true;
						m_salts[bucket.bucket_index] =
						  ( static_cast<salt_type>( i ) + 1 ) * -1;
						m_data[i].first = bucket.items[0]->first;
						m_data[i].second = bucket.items[0]->second;
						return;
					}
				}
			}

			for( salt_type salt = 0;; ++salt ) {
				daw::bounded_vector_t<hash_result, N> slots_this_bucket{};
				bool const success = daw::algorithm::all_of(
				  bucket.items.begin( ), bucket.items.end( ),
				  [&]( value_type const *item ) -> bool {
					  hash_result slot_wanted = call_hash( item->first, salt ) % N;
					  if( slots_claimed[static_cast<size_type>( slot_wanted )] ) {
						  return false;
					  }
					  if( daw::algorithm::find( slots_this_bucket.begin( ),
					                            slots_this_bucket.end( ), slot_wanted ) !=
					      slots_this_bucket.end( ) ) {
						  return false;
					  }
					  slots_this_bucket.push_back( slot_wanted );
					  return true;
				  } );
				if( success ) {
					m_salts[bucket.bucket_index] = salt;
					for( size_type i = 0; i < bucket.items.size( ); ++i ) {
						m_data[static_cast<size_type>( slots_this_bucket[i] )].first =
						  bucket.items[i]->first;
						m_data[static_cast<size_type>( slots_this_bucket[i] )].second =
						  bucket.items[i]->second;
						slots_claimed[static_cast<size_type>( slots_this_bucket[i] )] =
						  true;
					}
					return;
				}
			}
		}

		constexpr size_type find_data_index( Key const &key ) const {
			auto bucket = static_cast<size_type>( first_hash( key ) );
			auto salt = m_salts[static_cast<size_type>( bucket )];

			if( salt < 0 ) {
				return static_cast<size_type>( ( salt * -1 ) - 1 );
			}
			return static_cast<size_type>( call_hash( key, salt ) ) % N;
		}

	public:
		explicit constexpr perfect_hash_table(
		  std::pair<Key, Value> const ( &data )[N] ) {
			using buckets_t = std::array<bucket_t, m_num_buckets>;
			buckets_t buckets{};

			for( size_type i = 0; i < m_num_buckets; ++i ) {
				buckets[i].bucket_index = i;
			}

			for( auto const &p : data ) {
				auto bucket = static_cast<size_type>( first_hash( p.first ) );
				buckets[bucket].items.push_back( &p );
			}

			daw::sort(
			  buckets.begin( ), buckets.end( ),
			  []( bucket_t const &lhs, bucket_t const &rhs ) { return lhs > rhs; } );

			std::array<bool, N> slots_claimed{};
			for( auto &b : buckets ) {
				if( b.items.empty( ) ) {
					break;
				}
				find_salt_for_bucket( b, slots_claimed );
			}
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return m_data.data( );
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return m_data.data( );
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return m_data.data( );
		}

		[[nodiscard]] constexpr iterator end( ) noexcept {
			return m_data.data( ) + static_cast<ptrdiff_t>( N );
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return m_data.data( ) + static_cast<ptrdiff_t>( N );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return m_data.data( ) + static_cast<ptrdiff_t>( N );
		}

		[[nodiscard]] constexpr const_iterator find( Key const &key ) const {
			size_type const data_index = find_data_index( key );
			if( key_equal{}( m_data[data_index].first, key ) ) {
				return &m_data[data_index];
			}
			return end( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return N;
		}

		[[nodiscard]] constexpr bool contains( Key const &key ) const noexcept {
			size_type const data_index = find_data_index( key );
			return key_equal{}( m_data[data_index].first, key );
		}

		[[nodiscard]] constexpr iterator find( Key const &key ) {
			size_type const data_index = find_data_index( key );
			if( key_equal{}( m_data[data_index].first, key ) ) {
				return &m_data[data_index];
			}
			return end( );
		}

		[[nodiscard]] constexpr mapped_type &operator[]( Key const &key ) {
			return m_data[find_data_index( key )].second;
		}

		[[nodiscard]] constexpr mapped_type const &
		operator[]( Key const &key ) const {
			return m_data[find_data_index( key )].second;
		}
	}; // namespace daw

	// *****************************************************
	// Perfect Hash Table Iterator
	template<size_t N, typename Key, typename Value, typename Hasher,
	         typename KeyEqual>
	class perfect_hash_table_iterator {
		using hash_table_t = perfect_hash_table<N, Key, Value, Hasher, KeyEqual>;
		friend hash_table_t;

	public:
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using value_type = typename hash_table_t::value_type;
		using pointer = std::add_pointer_t<value_type>;
		using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
		using iterator_category = std::random_access_iterator_tag;
		using reference = std::add_lvalue_reference_t<value_type>;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<value_type>>;

	private:
		pointer m_item;
		constexpr perfect_hash_table_iterator( pointer item )
		  : m_item( item ) {}

		[[nodiscard]] constexpr decltype( auto ) ptr( ) {
			return m_item;
		}

		[[nodiscard]] constexpr decltype( auto ) ptr( ) const {
			return m_item;
		}

	public:
		[[nodiscard]] constexpr reference operator*( ) {
			return *ptr( );
		}

		[[nodiscard]] constexpr pointer operator->( ) {
			return ptr( );
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *ptr( );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const {
			return ptr( );
		}

		constexpr perfect_hash_table_iterator &operator++( ) {
			++m_item;
			return *this;
		}

		constexpr perfect_hash_table_iterator operator++( int ) & {
			perfect_hash_table_iterator result = *this;
			++m_item;
			return result;
		}

		constexpr perfect_hash_table_iterator &operator--( ) {
			--m_item;
			return *this;
		}

		constexpr perfect_hash_table_iterator operator--( int ) & {
			perfect_hash_table_iterator result = *this;
			--m_item;
			return result;
		}

		[[nodiscard]] constexpr perfect_hash_table_iterator &
		operator+=( difference_type n ) {
			m_item += n;
			return *this;
		}

		[[nodiscard]] constexpr perfect_hash_table_iterator &
		operator-=( difference_type n ) {
			m_item -= n;
			return *this;
		}

		[[nodiscard]] constexpr perfect_hash_table_iterator
		operator+( difference_type n ) const noexcept {
			perfect_hash_table_iterator result = *this;
			m_item += n;
			return result;
		}

		[[nodiscard]] constexpr perfect_hash_table_iterator
		operator-( difference_type n ) const noexcept {
			perfect_hash_table_iterator result = *this;
			m_item -= n;
			return result;
		}

		[[nodiscard]] constexpr reference operator[]( size_type n ) noexcept {
			return *( m_item + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] constexpr const_reference operator[]( size_type n ) const
		  noexcept {
			return *( m_item + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] constexpr friend bool
		operator==( perfect_hash_table_iterator const &lhs,
		            perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item == rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator!=( perfect_hash_table_iterator const &lhs,
		            perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item != rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator<( perfect_hash_table_iterator const &lhs,
		           perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item < rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator<=( perfect_hash_table_iterator const &lhs,
		            perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item <= rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator>( perfect_hash_table_iterator const &lhs,
		           perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item > rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator>=( perfect_hash_table_iterator const &lhs,
		            perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item >= rhs.m_item;
		}
	};

	template<size_t N, typename Key, typename Value, typename Hasher,
	         typename KeyEqual>
	class const_perfect_hash_table_iterator {
		using hash_table_t = perfect_hash_table<N, Key, Value, Hasher, KeyEqual>;
		friend hash_table_t;
		using pointer = typename hash_table_t::value_type const *;

	public:
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using value_type = typename std::add_const_t<hash_table_t>::value_type;
		using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
		using iterator_category = std::random_access_iterator_tag;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<value_type>>;

	private:
		pointer m_item;
		constexpr const_perfect_hash_table_iterator( pointer item )
		  : m_item( item ) {}

		[[nodiscard]] constexpr decltype( auto ) ptr( ) const {
			return m_item;
		}

	public:
		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *ptr( );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const {
			return ptr( );
		}

		constexpr const_perfect_hash_table_iterator &operator++( ) {
			++m_item;
			return *this;
		}

		constexpr const_perfect_hash_table_iterator operator++( int ) & {
			const_perfect_hash_table_iterator result = *this;
			++m_item;
			return result;
		}
		constexpr const_perfect_hash_table_iterator &operator--( ) {
			--m_item;
			return *this;
		}

		constexpr const_perfect_hash_table_iterator operator--( int ) & {
			const_perfect_hash_table_iterator result = *this;
			--m_item;
			return result;
		}

		[[nodiscard]] constexpr const_perfect_hash_table_iterator &
		operator+=( difference_type n ) {
			m_item += n;
			return *this;
		}

		[[nodiscard]] constexpr const_perfect_hash_table_iterator &
		operator-=( difference_type n ) {
			m_item -= n;
			return *this;
		}

		[[nodiscard]] constexpr const_perfect_hash_table_iterator
		operator+( difference_type n ) const noexcept {
			const_perfect_hash_table_iterator result = *this;
			m_item += n;
			return result;
		}

		[[nodiscard]] constexpr const_perfect_hash_table_iterator
		operator-( difference_type n ) const noexcept {
			const_perfect_hash_table_iterator result = *this;
			m_item -= n;
			return result;
		}

		[[nodiscard]] constexpr const_reference operator[]( size_type n ) const
		  noexcept {
			return *( m_item + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] constexpr friend bool
		operator==( const_perfect_hash_table_iterator const &lhs,
		            const_perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item == rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator!=( const_perfect_hash_table_iterator const &lhs,
		            const_perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item != rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator<( const_perfect_hash_table_iterator const &lhs,
		           const_perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item < rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator<=( const_perfect_hash_table_iterator const &lhs,
		            const_perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item <= rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator>( const_perfect_hash_table_iterator const &lhs,
		           const_perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item > rhs.m_item;
		}

		[[nodiscard]] constexpr friend bool
		operator>=( const_perfect_hash_table_iterator const &lhs,
		            const_perfect_hash_table_iterator const &rhs ) {
			return lhs.m_item >= rhs.m_item;
		}
	};
} // namespace daw
