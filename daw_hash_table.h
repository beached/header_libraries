// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#include <algorithm>
#include <functional>
#include <list>
#include <numeric>
#include <string>
#include <utility>

#include "daw_array.h"
#include "daw_exception.h"
#include "daw_utility.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename ValueType>
			struct hash_table_item {
				using value_type = typename daw::traits::root_type_t<ValueType>;
				size_t hash;	// 0 is the sentinel to mark unused
				value_type value;

				void clear( ) {
					hash = 0;
					value = value_type{ };
				}

				bool empty( ) const noexcept {
					return 0 == hash;
				}

				explicit operator bool( ) const {
					return 0 != hash;
				}

				hash_table_item( ):
					hash{ 0 },
					value{ } { }

				~hash_table_item( ) = default;
				hash_table_item( hash_table_item const & ) = default;
				hash_table_item( hash_table_item && ) = default;
				hash_table_item & operator=( hash_table_item const & ) = default;
				hash_table_item & operator=( hash_table_item && ) = default;
			};	// struct hash_table_item

	}	// namespace impl

	template<typename Value>
		struct hash_table {
			static constexpr const double ResizeRatio=2.00;	// This will leave the hash at 2/3 full
			using value_type = daw::traits::root_type_t<Value>;
			using reference = value_type &;
			using const_reference = value_type const &;
		private:
			using values_type = daw::array<impl::hash_table_item<value_type>>;
			using iterator = typename values_type::iterator;
			values_type m_values;
			size_t m_occupancy;

			auto begin( ) {
				return m_values.begin( );
			}

			auto begin( ) const {
				return m_values.begin( );
			}

			auto end( ) {
				return m_values.end( );
			}

			auto end( ) const {
				return m_values.end( );
			}
		public:
			hash_table( ): 
				m_values{ 7 },
				m_occupancy{ 0 } { }

			~hash_table( ) = default;
			hash_table( hash_table && ) = default;
			hash_table( hash_table const & ) = default;
			hash_table & operator=( hash_table && ) = default;
			hash_table & operator=( hash_table const & ) = default;

		private:
			template<typename KeyType>
				static size_t hash_fn( KeyType && key ) {
					static const auto s_hash = []( auto && k ) {
						using k_type = daw::traits::root_type_t<KeyType>;
						static std::hash<k_type> h_func;
						return (h_func( ::std::forward<KeyType>( k ) ) % max_size( )) + 1;	// Guarantee we cannot be zero
					};
					return s_hash( ::std::forward<KeyType>( key ) );
				}

			static size_t hash_fn( char const * c_str ) {
				std::string value = c_str;
				return hash_fn( value );
			}

			static size_t scale_hash( size_t hash, size_t table_size ) {
				// Scale value to capacity using MAD(Multiply-Add-Divide) compression
				// Use the two largest Prime's that fit in a 64bit unsigned integral
				assert( table_size < max_size( ) );	// Table size must be less than max of ptrdiff_t as we use the value 0 as a sentinel.  This should be rare
				assert( hash != 0 );	// zero is a sentinel for no value
				static const size_t prime_a = 18446744073709551557u;
				static const size_t prime_b = 18446744073709551533u;
				return ((hash*prime_a + prime_b) % table_size);
			}

			static size_t resize_table( values_type & old_table, size_t new_size ) {
				values_type new_hash_table{ new_size };
				size_t occupancy = 0;
				for( auto && current_item: old_table ) {
					if( current_item ) {
						++occupancy;
						auto pos = find_item_by_hash( current_item.hash, new_hash_table );
						assert( pos != new_hash_table.end( ) );
						using std::swap;
						swap( *pos, current_item );
					}
				}
				old_table = std::move( new_hash_table );
				return occupancy;
			}

			static auto find_item_by_hash( size_t hash, values_type const & tbl ) {
				auto const scaled_hash = static_cast<ptrdiff_t>(scale_hash( hash, tbl.size( ) ));
				auto const start_it = std::next( tbl.begin( ), scaled_hash );
				// loop through all values until an empty spot is found(at end start at beginning)
				auto const is_here = [hash]( auto const & item ) {
					return item.empty( ) || hash == item.hash;
				};
				auto pos = std::find_if( start_it, tbl.end( ), is_here );
				if( tbl.end( ) == pos ) {
					pos = std::find_if( tbl.begin( ), start_it, is_here );
					if( start_it == pos ) {
						pos = tbl.end( );
					}
				}
				return const_cast<iterator>( pos );
			}

			static auto find_item_by_hash( size_t hash, hash_table const & tbl ) {
				return find_item_by_hash( hash, tbl.m_values );
			}

			static iterator find_item_by_hash_or_create( size_t hash, hash_table & tbl ) {
				if( ((tbl.m_occupancy*100)/tbl.m_values.size( )) > 70 ) {
					tbl.grow_table();
				}
				auto pos = find_item_by_hash( hash, tbl.m_values );
				if( pos == tbl.end( ) ) {
					tbl.grow_table( );
					pos = find_item_by_hash( hash, tbl );
					daw::exception::daw_throw_on_true( pos == tbl.end( ) );
					// This is a WAG.  It be wrong but I had to pick something
					// without evidence to support it.
				}
				if( pos->empty( ) ) {
					++tbl.m_occupancy;
					pos->hash = hash;
				}
				return pos;
			}

			static auto insert_into( impl::hash_table_item<value_type> && item, hash_table & tbl ) {
				auto pos = find_item_by_hash_or_create( item.hash, tbl );
				*pos == std::move( item );
			}

			static auto insert_into( size_t hash, value_type value, hash_table & tbl ) {
				auto pos = find_item_by_hash_or_create( hash, tbl );
				pos->value = std::move( value );
			}

			void grow_table( ) {
				m_occupancy = resize_table( m_values, static_cast<size_t>(static_cast<double>(m_values.size( )) * ResizeRatio) );
			}

		public:
			static size_t max_size( ) {
				return static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max( ) - 1);
			}

			template<typename Key>
				auto insert( Key const & key, value_type value ) {
				}

			template<typename Key>
				auto & operator[]( Key const & key ) {
					auto hash = hash_fn( key );
					auto pos = find_item_by_hash_or_create( hash, *this );
					return pos->value;
				}

			template<typename Key>
				auto const & operator[]( Key const & key ) const {
					auto hash = hash_fn( key );
					auto pos = find_item_by_hash( hash, *this );
					if( m_values.end( ) == pos ) {
						throw std::out_of_range( "Key does not already exist" );
					}
					return pos->value;
				}

			template<typename Key>
				size_t erase( Key && key ) {
					auto hash = hash_fn( key );
					auto pos = find_item_by_hash( hash, m_values );
					pos->clear( );
					return 1;
				}

			void shrink_to_fit( ) {
				auto count = std::accumulate( m_values.begin( ), m_values.end( ), 0u, []( auto const & a, auto const & b ) {
					return a + (b.empty( ) ? 0 : 1);
				} );
				resize_table( m_values, count );
			}

			size_t count_occupied( ) const {
				m_occupancy = std::accumulate( m_values.begin( ), m_values.end( ), static_cast<size_t>(0), []( auto const & result, auto const & cur_val ) {
					return result + (cur_val.empty( ) ? 0 : 1 );
				});

				return m_occupancy;
			}

			size_t capacity( ) const {
				return m_values.size( );
			}
		};	// struct hash_table

}	// namespace daw

