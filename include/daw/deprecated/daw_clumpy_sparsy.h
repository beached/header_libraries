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

#include <cstdint>
#include <vector>

namespace daw {
	template<typename T>
	struct clumpy_sparsy_iterator;

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

			[[nodiscard]] size_t size( ) const {
				return m_items.size( );
			}

			[[nodiscard]] size_t &start( ) {
				return m_start;
			}

			[[nodiscard]] size_t const &start( ) const {
				return m_start;
			}

			[[nodiscard]] size_t end( ) const {
				return m_start + size( );
			}

			[[nodiscard]] std::vector<T> &items( ) {
				return m_items;
			}

			[[nodiscard]] std::vector<T> const &items( ) const {
				return m_items;
			}

			[[nodiscard]] bool operator<( Chunk const &rhs ) {
				return start( ) < rhs.start( );
			}
		}; // class Chunk
	public:
		using values_type = std::vector<Chunk>;
		using value_type = typename values_type::value_type;
		using reference = typename values_type::reference;
		using const_reference = typename values_type::reference;
		using iterator = clumpy_sparsy_iterator<T>;
		using const_iterator = clumpy_sparsy_const_iterator<T>;

	private:
		// Must be mutable as we hide our size and expand the values < size to fit
		// as needed
		values_type mutable m_items;
		size_t m_size;

		[[nodiscard]] auto lfind( size_t pos ) {
			if( pos >= size( ) ) {
				return size( );
			}
			auto item_pos = daw::algorithm::find_last_of(
			  m_items.begin( ), m_items.end( ), [pos]( auto const &item ) {
				  return pos >= item.start( );
			  } );
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
		[[nodiscard]] size_t size( ) const {
			return m_size;
		}

		[[nodiscard]] reference operator[]( size_t pos ) {
			auto item = lfind( pos );
		}

		[[nodiscard]] iterator begin( ) {
			return clumpy_sparsy_iterator<T>( this );
		}

		[[nodiscard]] const_iterator begin( ) const {
			return clumpy_sparsy_const_iterator<T>( this );
		}

		[[nodiscard]] const_iterator cbegin( ) const {
			return clumpy_sparsy_const_iterator<T>( this );
		}

		[[nodiscard]] iterator end( ) {
			return clumpy_sparsy_iterator<T>( this, size( ) );
		}

		[[nodiscard]] const_iterator end( ) const {
			return clumpy_sparsy_const_iterator<T>( this, size( ) );
		}

		[[nodiscard]] const_iterator cend( ) const {
			return clumpy_sparsy_const_iterator<T>( this, size( ) );
		}

	}; // class clumpy_sparsy

	template<typename T>
	struct clumpy_sparsy_iterator {
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using value_type = typename clumpy_sparsy<T>::value_type;
		using pointer = std::add_pointer_t<value_type>;
		using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
		using iterator_category = std::random_access_iterator_tag;
		using reference = typename clumpy_sparsy<T>::reference;
		using const_reference = typename clumpy_sparsy<T>::const_reference;

	private:
		size_t m_position = ( std::numeric_limits<size_t>::max )( );
		clumpy_sparsy<T> *m_items = nullptr;

	public:
		clumpy_sparsy_iterator( ) = default;

		constexpr clumpy_sparsy_iterator( clumpy_sparsy<T> *items,
		                                  size_t position = 0 ) noexcept
		  : m_position( items ? position : ( std::numeric_limits<size_t>::max )( ) )
		  , m_items( items ? items : nullptr ) {}

	private:
		[[nodiscard]] auto as_tuple( ) {
			return std::tie( m_position, m_items );
		}

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

		[[nodiscard]] bool equal( clumpy_sparsy_iterator const &other ) {
			return as_tuple( ) == other.as_tuple( );
		}

		[[nodiscard]] decltype( auto ) dereference( ) const {
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

		[[nodiscard]] reference operator*( ) {
			return dereference( );
		}

		[[nodiscard]] pointer operator->( ) {
			return &dereference( );
		}

		[[nodiscard]] const_reference operator*( ) const {
			return dereference( );
		}

		[[nodiscard]] const_pointer operator->( ) const {
			return &dereference( );
		}

		clumpy_sparsy_iterator &operator++( ) {
			increment( );
			return *this;
		}

		clumpy_sparsy_iterator operator++( int ) {
			auto result = *this;
			increment( );
			return result;
		}

		clumpy_sparsy_iterator &operator--( ) {
			decrement( );
			return *this;
		}

		clumpy_sparsy_iterator operator--( int ) {
			clumpy_sparsy_iterator result = *this;
			decrement( );
			return result;
		}

		clumpy_sparsy_iterator &operator+=( difference_type n ) {
			advance( n );
			return *this;
		}

		clumpy_sparsy_iterator &operator-=( difference_type n ) {
			advance( -n );
			return *this;
		}

		clumpy_sparsy_iterator operator+( difference_type n ) const noexcept {
			auto result = *this;
			advance( n );
			return result;
		}

		clumpy_sparsy_iterator operator-( difference_type n ) const noexcept {
			clumpy_sparsy_iterator result = *this;
			advance( -n );
			return result;
		}

		[[nodiscard]] reference operator[]( size_type n ) noexcept {
			return ( *m_items )[m_position + n];
		}

		[[nodiscard]] const_reference operator[]( size_type n ) const noexcept {
			return ( *m_items )[m_position + n];
		}

		friend bool operator==( clumpy_sparsy_iterator const &lhs,
		                        clumpy_sparsy_iterator const &rhs ) {
			return lhs.ptr == rhs.ptr;
		}

		friend bool operator!=( clumpy_sparsy_iterator const &lhs,
		                        clumpy_sparsy_iterator const &rhs ) {
			return lhs.ptr != rhs.ptr;
		}

		friend bool operator<( clumpy_sparsy_iterator const &lhs,
		                       clumpy_sparsy_iterator const &rhs ) {
			return lhs.ptr < rhs.ptr;
		}

		friend bool operator<=( clumpy_sparsy_iterator const &lhs,
		                        clumpy_sparsy_iterator const &rhs ) {
			return lhs.ptr <= rhs.ptr;
		}

		friend bool operator>( clumpy_sparsy_iterator const &lhs,
		                       clumpy_sparsy_iterator const &rhs ) {
			return lhs.ptr > rhs.ptr;
		}

		friend bool operator>=( clumpy_sparsy_iterator const &lhs,
		                        clumpy_sparsy_iterator const &rhs ) {
			return lhs.ptr >= rhs.ptr;
		}
	}; // class clumpy_sparsy_iterator

} // namespace daw
