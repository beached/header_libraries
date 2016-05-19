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
#include <vector>

namespace daw {

	template<typename Value, double ResizeRatio=1.15>
	struct hash_table {
		using value_type = typename ::std::decay_t<Value>;
		using reference = value_type &;
		using const_reference = value_type const &;
	private:
		struct hash_table_item {
			bool m_occupied;
			::std::size_t m_hash;
			using ptr_t = value_type *;
			ptr_t m_pos;
	public:	
			auto operator( )( ) -> ::std::enable_if_t<sizeof(value_type) <= sizeof( ptr_t ), reference> {
				return reinterpret_cast<value_type>( m_pos )
			}

			auto operator( )( ) -> ::std::enable_if_t<sizeof(value_type) <= sizeof( ptr_t ), const_reference> const {
				return reinterpret_cast<value_type>( m_pos )
			}

			auto operator( )( ) -> ::std::enable_if_t<sizeof(value_type) > sizeof( ptr_t ), reference> {
				return *m_pos;
			}

			auto operator( )( ) -> ::std::enable_if_t<sizeof(value_type) > sizeof( ptr_t ), const_reference> const {
				return *m_pos;
			}

			hash_table_item( ): m_occupied{ false }, m_hash{ 0 }, m_pos{ nullptr } { }

			template<typename = ::std::enable_if_t<sizeof(value_type) <= sizeof( ptr_t ), void>
			hash_table_item( ::std::size_t hash, value_type const & value, bool occupied = true ): m_occupied{ occupied }, m_hash{ hash }, pos{ reinterpret_cast<ptr_t>( v ) } { }

			template<typename = ::std::enable_if_t<sizeof(value_type) > sizeof( ptr_t ), void>
			hash_table_item( ::std::size_t hash, value_type value, bool occupied = true ): m_occupied{ occupied }, m_hash{ hash }, pos{ new value_type( ::std::move( v ) ) } { }
			
			template<typename = ::std::enable_if_t<sizeof(value_type) <= sizeof( ptr_t ), void>
			~hash_table_item( ) {
				reinterpret_cast<value_type>( m_pos ).~value_type( );	
			}

			template<typename = ::std::enable_if_t<sizeof(value_type) > sizeof( ptr_t ), void>
			~hash_table_item( ) {
				if( nullptr != m_pos ) {
					delete m_pos;
				}
			}

			hash_table_item( hash_table_item && ) = default;
			hash_table_item & operator=( hash_table_item && ) = default;

		};	// struct hash_table_item

		using values_type = ::std::vector<hash_table_item>;
		using iterator = values_type::iterator;
		values_type m_values;

	public:
		hash_table( ): m_values{ 1000 } }, m_hash{ } { }

		~hash_table( ) = default;
		hash_table( hash_table && ) = default;
		hash_table( hash_table const & ) = default;
		hash_table & operator=( hash_table && ) = default;
		hash_table & operator=( hash_table const & ) = default;

	private:
		template<typename Key>
		static iterator find_item( Key const & key, values_type const & tbl ) {
			static constexpr auto const hash = ::std::hash<Key>{ }; 
			auto hash_value = hash( key );
			auto hash_it = tbl.begin( ) + (hash_value % tbl.size( ));	
			auto count = tbl.size( );
			// loop through all values until an empty spot is found(at end start at beginning)
			while( count-- > 0 ) {	
				if( !hash_it->occupied ) {
					return hash_it;
				} else if( hash_it->hash == hash_value ) {
					return hash_it;
				}
				if( tbl.end( ) == ++hash_it ) {
					hash_it = tbl.begin( );
				}
			}
			// Full
			return tbl.end( );
		}

		static void resize_table( values_type & old_table ) {
			values_type new_hash_table{ static_cast<size_t>( static_cast<double>( m_hash_table.size( ) ) * ResizeRatio ) };
			for( auto & current_item: m_table ) {
				auto value = current_item
				insert_into( current_item. 
			}
		}


		static iterator itsert_into( value_type item, values_type & tbl ) {
			auto item_it = find_item( item.first, tbl );
			if( tbl.end( ) == item_it ) {
				// resize table
			}
			auto hash_it = m_values.begin( ) + (hash( key ) % m_values.size( ));	
			while( !hash_it->occupied ) {
				++hash_it;
				if( m_values.end( ) == hash_it ) {


			if( tbl.end( ) != item_it ) {
				item.occupied = true;

			}
			return item_it;
		}

		static iterator_out insert_into( key_type key, value_item_type value, values_type & tbl ) {
			auto item = find_item( key );
			if( tbl.end( ) != item ) {
				item.occupied = true;
				item.value = value_type( ::std::move( key ), ::std::move( value ) );
			}
			return item; 
		}

	public:
		iterator insert( key_type key, value_item_type value ) {
			auto result = insert_into( std::move( key ), std::move( value ), m_table );
			if( m_table.end( ) == result ) {
				resize_table( );
				result = insert_at( hash_pos, std::move( key ), std::move( value ), m_table );

				if( m_table.end( ) == result ) {
					// Should not happen, probably out of memory
					throw ::std::runtime_error( "Unknown error while increases hash table size.  Likely out of memory" );
				}
			}
			return &result.value;
		}

	};	// struct hash_table

}	// namespace daw

