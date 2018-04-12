// The MIT License (MIT)
//
// Copyright (c) 2013-2018 Darrell Wright
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

#include "daw_algorithm.h"
#include <boost/iterator/iterator_facade.hpp>
#include <cstdint>
#include <vector>

namespace daw {
	template<typename T>
	class clumpy_sparsy_iterator;

	template<typename T>
	using clumpy_sparsy_const_iterator = clumpy_sparsy_iterator<T const>;

	// Provide a vector like structure that assumes that the sparseness has clumps
	template<typename T>
	class clumpy_sparsy {
		class Chunk {
			size_t m_start;
			std::vector<T> m_items;

		public:
			Chunk( ) = default;
			~Chunk( ) = default;
			Chunk( Chunk const & ) = default;
			Chunk( Chunk && ) = default;
			Chunk &operator=( Chunk const & ) = default;
			Chunk &operator=( Chunk && ) = default;
			size_t size( ) const {
				return m_items.size( );
			}

			size_t &start( ) {
				return m_start;
			}

			size_t const &start( ) const {
				return m_start;
			}

			size_t end( ) const {
				return m_start + size( );
			}

			std::vector<T> &items( ) {
				return m_items;
			}

			std::vector<T> const &items( ) const {
				return m_items;
			}

			bool operator<( Chunk const &rhs ) {
				return start( ) < rhs.start( );
			}
		}; // class Chunk
	public:
		using values_type = ::std::vector<Chunk>;
		using value_type = typename values_type::value_type;
		using reference = typename values_type::reference;
		using const_reference = typename values_type::reference;
		using iterator = clumpy_sparsy_iterator<T>;
		using const_iterator = clumpy_sparsy_const_iterator<T>;

	private:
		// Must be mutable as we hide our size and expand the values < size to fit as needed
		values_type mutable m_items;
		size_t m_size;

		auto lfind( size_t pos ) {
			if( pos >= size( ) ) {
				return size( );
			}
			auto item_pos = daw::algorithm::find_last_of( m_items.begin( ), m_items.end( ),
			                                              [pos]( auto const &item ) { return pos >= item.start( ); } );
			if( item_pos < size( ) ) {
				// We are within the max size of the container.  Feel free to add it
				auto offset = pos - item_pos->start( );
				if( offset > item_pos->size( ) + 1 ) {
					++item_pos;
					item_pos = m_items.emplace( item_pos );
					item_pos->start( ) = pos;
					item_pos->items( ).emplace_back( );
				}
			}
			return item_pos;
		}

	public:
		size_t size( ) const {
			return m_size;
		}

		reference operator[]( size_t pos ) {
			auto item = lfind( pos );
		}

		iterator begin( ) {
			return clumpy_sparsy_iterator<T>( this );
		}

		const_iterator begin( ) const {
			return clumpy_sparsy_const_iterator<T>( this );
		}

		const_iterator cbegin( ) const {
			return clumpy_sparsy_const_iterator<T>( this );
		}

		iterator end( ) {
			return clumpy_sparsy_iterator<T>( this, size( ) );
		}

		const_iterator end( ) const {
			return clumpy_sparsy_const_iterator<T>( this, size( ) );
		}

		const_iterator cend( ) const {
			return clumpy_sparsy_const_iterator<T>( this, size( ) );
		}

	}; // class clumpy_sparsy

	template<typename T>
	class clumpy_sparsy_iterator
	  : public boost::iterator_facade<clumpy_sparsy_iterator<T>, T, boost::random_access_traversal_tag> {
		size_t m_position;
		clumpy_sparsy<T> *m_items;

		auto as_tuple( ) {
			return std::tie( m_position, m_items );
		}

		friend class boost::iterator_core_access;
		void increment( ) {
			if( m_items->size( ) == m_position ) {
				return;
			}
			++m_position;
		}

		void decrement( ) {
			if( 0 == m_position ) {
				return;
			}
			--m_position;
		}

		bool equal( clumpy_sparsy_iterator const &other ) {
			return as_tuple( ) == other.as_tuple( );
		}

		auto &dereference( ) const {
			return ( *m_items )[m_position];
		}

		void advance( ptrdiff_t n ) {
			if( 0 > n ) {
				n *= -1;
				if( n >= m_position ) {
					m_position = 0;
				} else {
					m_position -= n;
				}
			} else {
				m_position += n;
				{
					auto sz = m_items->size( );
					if( sz < m_position ) {
						m_position = sz;
					}
				}
			}
		}

	public:
		clumpy_sparsy_iterator( )
		  : m_position( std::numeric_limits<size_t>::max( ) )
		  , m_items( nullptr ) {}
		clumpy_sparsy_iterator( clumpy_sparsy<T> *items, size_t position = 0 )
		  : m_position( position )
		  , m_items( items ) {}
	}; // class clumpy_sparsy_iterator

} // namespace daw
