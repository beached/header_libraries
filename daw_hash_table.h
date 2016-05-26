// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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
#include <list>
#include <utility>
#include <string>
#include <vector>
#include "daw_utility.h"

namespace daw {
	namespace impl {
		template<typename ValueType, typename = void>
		struct hash_table_item;

		// Small value optimized version
		template<typename ValueType>
		struct hash_table_item<ValueType, ::std::enable_if_t<(sizeof( ValueType ) <= sizeof( ValueType * ))>> {
			using value_type = typename ::std::decay_t<ValueType>;
			using reference = value_type &;
			using const_reference = value_type const &;
		private:
			size_t m_hash;	// 0 is the sentinel to mark unused
			value_type m_value;
		public:	
			hash_table_item( ): m_hash{ 0 }, m_value{ } { }

			hash_table_item( size_t hash, value_type value, bool occupied = true ): m_hash{ hash }, m_value{ ::std::move( value ) } { }

			~hash_table_item( ) = default;
			hash_table_item( hash_table_item const & ) = default;
			hash_table_item( hash_table_item && ) = default;
			hash_table_item & operator=( hash_table_item const & ) = default;
			hash_table_item & operator=( hash_table_item && ) = default;

			friend void swap( hash_table_item & lhs, hash_table_item & rhs ) noexcept {
				using ::std::swap;
				swap( lhs.m_hash, rhs.m_hash );
				swap( lhs.m_value, rhs.m_value );
			}
			
			void clear( ) {
				m_hash = 0;
				m_value = 0;
			}

			bool occupied( ) const noexcept {
				return 0 != m_hash;
			}

			size_t & hash( ) noexcept {
				return m_hash;
			}

			size_t const & hash( ) const noexcept {
				return m_hash;
			}

			reference value( ) noexcept {
				return m_value;
			}

			const_reference value( ) const noexcept {
				return m_value;
			}

			explicit operator bool( ) const {
				return occupied( );
			}
		};	// struct hash_table_item

		// Values that are larger than the pointer size(e.g. 32bit/64bit) are heap allocated.
		template<typename ValueType>
		struct hash_table_item<ValueType, ::std::enable_if_t<(sizeof( ValueType ) > sizeof( ValueType * ))>> {
			using value_type = typename ::std::decay_t<ValueType>;
			using reference = value_type &;
			using const_reference = value_type const &;
		private:
			size_t m_hash;	// 0 is the sentinel to mark unused
			value_type * m_value;
		public:	
			hash_table_item( ) noexcept: m_hash{ 0 }, m_value{ nullptr } { }

			hash_table_item( size_t hash, value_type value ): m_hash{ hash }, m_value{ new value_type( ::std::move( value ) ) } { }
			
			~hash_table_item( ) {
				clear( );
			}

			hash_table_item( hash_table_item const & other ): m_hash{ other.m_hash }, m_value{ ::daw::copy_ptr_value( other.m_value ) } { }

			friend void swap( hash_table_item & lhs, hash_table_item & rhs ) noexcept {
				using ::std::swap;
				swap( lhs.m_hash, rhs.m_hash );
				swap( lhs.m_value, rhs.m_value );
			}

			hash_table_item & operator=( hash_table_item rhs ) noexcept {
				swap( *this, rhs );
				return *this;
			}

			hash_table_item( hash_table_item && other ) noexcept: hash_table_item{ } {
				swap( *this, other );
			}
			
			void clear( ) {
				m_hash = 0;
				auto tmp = m_value;
				m_value = nullptr;
				if( nullptr != tmp ) {
					delete tmp;
				}
			}

			bool occupied( ) const noexcept {
				return 0 != m_hash;
			}

			size_t & hash( ) noexcept {
				return m_hash;
			}

			size_t const & hash( ) const noexcept {
				return m_hash;
			}

			reference value( ) noexcept {
				return *m_value;
			}

			const_reference value( ) const noexcept {
				return *m_value;
			}

			explicit operator bool( ) const {
				return occupied( );
			}
		};	// struct hash_table_item
	}	// namespace impl

	template<typename Value>
	struct hash_table {
		static constexpr const double ResizeRatio=1.15;
		using value_type = typename ::std::decay_t<Value>;
		using reference = value_type &;
		using const_reference = value_type const &;
	private:
		using values_type = ::std::vector<impl::hash_table_item<value_type>>;
		using iterator = typename values_type::iterator;
		values_type m_values;
	public:
		hash_table( ): m_values{ 11 } { }

		~hash_table( ) = default;
		hash_table( hash_table && ) = default;
		hash_table( hash_table const & ) = default;
		hash_table & operator=( hash_table && ) = default;
		hash_table & operator=( hash_table const & ) = default;

	private:
		template<typename Key>
		static size_t hash_fn( Key && key ) {
			static const auto s_hash = []( auto && k ) {
				using k_type = std::decay_t<Key>;
				static std::hash<k_type> h_func;
				return (h_func( ::std::forward<Key>( k ) ) % (static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max( )) - 1)) + 1;	// Guarantee we cannot be zero
			};			
			return s_hash( ::std::forward<Key>( key ) );
		}
		
		static size_t hash_fn( char const * c_str ) {
			std::string value = c_str;
			return hash_fn( value );	
		}

		static size_t scale_hash( size_t hash, size_t table_size ) {
			// Scale value to capacity using MAD(Multiply-Add-Divide) compression
			// Use the two largest Prime's that fit in a 64bit unsigned integral
			assert( table_size < static_cast<size_t>( std::numeric_limits<ptrdiff_t>::max( )) );	// Table size must be less than max of ptrdiff_t as we use the value 0 as a sentinel.  This should be rare
			assert( hash != 0 );	// zero is a sentinel for no value
			static const size_t prime_a = 18446744073709551557u;
			static const size_t prime_b = 18446744073709551533u;
			return ((hash*prime_a + prime_b) % table_size);
		}

		template<typename TableType>
		static auto find_item_by_hash( size_t hash, TableType & tbl ) {
			auto hash_it = tbl.begin( ) + static_cast<ptrdiff_t>( scale_hash( hash, tbl.size( ) ) );	// scaled hashes are never 0 and never >= ptrdiff_t max
			auto count = tbl.size( );
			// loop through all values until an empty spot is found(at end start at beginning)
			while( count-- > 0 ) {	
				if( !hash_it->occupied( ) ) {
					return hash_it;
				} else if( hash_it->hash( ) == hash ) {
					return hash_it;
				}
				if( tbl.end( ) == ++hash_it ) {
					hash_it = tbl.begin( );
				}
			}
			// Full
			return tbl.end( );
		}

		static void grow_table( values_type & old_table, size_t new_size ) {
			values_type new_hash_table{ new_size };
			for( auto & current_item: old_table ) {
				if( current_item ) {
					insert_into( current_item.hash( ), current_item.value( ), new_hash_table );
				}
			}
			old_table = new_hash_table;
		}

		static auto insert_into( iterator item, value_type value, values_type & tbl ) {
			assert( tbl.end( ) != item );
			assert( item->occupied( ) == true );
			item->value( ) = ::std::move( value );
			return item; 
		}

		static auto insert_into( size_t hash, value_type value, values_type & tbl ) {
			return insert_into( find_item_by_hash( hash, tbl ), ::std::move( value ), tbl );
		}

	public:
		template<typename Key>
		auto insert( Key const & key, value_type value ) {
			return m_values.end( ) == insert_into( hash_fn( key ), std::move( value ), m_values );
		}

		template<typename Key>
		auto & operator[]( Key const & key ) {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, m_values );
			if( m_values.end( ) == pos ) {
				grow_table( m_values, static_cast<size_t>( static_cast<double>( m_values.size( ) ) * ResizeRatio ) );
				pos = find_item_by_hash( hash, m_values );
				assert( m_values.end( ) != pos );
			}
			pos->hash( ) = hash;
			return pos->value( );
		}

		template<typename Key>
		auto const & operator[]( Key const & key ) const {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, m_values );
			if( m_values.end( ) == pos ) {
				grow_table( m_values, static_cast<size_t>(static_cast<double>(m_values.size( )) * ResizeRatio) );
				pos = find_item_by_hash( hash, m_values );
				assert( m_values.end( ) != pos );
			}
			if( !pos->occupied( ) ) {
				throw std::out_of_range( "Key does not already exist" );
			}
			return pos->value( );
		}

		template<typename Key>
		size_t erase( Key && key ) {
			auto hash = hash_fn( key );
			auto pos = find_item_by_hash( hash, m_values );
			pos->clear( );
			return 1;
		}
	};	// struct hash_table

}	// namespace daw

