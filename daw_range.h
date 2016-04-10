// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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

#include <iterator>

#include "daw_traits.h"

namespace daw {
	namespace range {
		template<typename Iterator>
		class Range {
			Iterator m_begin;
			Iterator m_end;
		public:
			using value_type = typename std::iterator_traits<Iterator>::value_type;
			using reference = typename std::iterator_traits<Iterator>::reference;
			using const_reference = const reference;
			using iterator = Iterator;
			using const_iterator = const iterator;
			using difference_type = typename std::iterator_traits<Iterator>::difference_type;
			Range( ) = default;
			Range( Range const & ) = default;
			Range( Range && ) = default;
			~Range( ) = default;
			Range & operator=( Range const & ) = default;
			Range & operator=( Range && ) = default;

			Range( Iterator First, Iterator Last ):				
				m_begin( First ),
				m_end( Last ) { 
			}

			Range& move_next( ) {
				assert( m_begin != m_end );
				++m_begin;
				return *this;
			}

			Range& move_back( ) {
				--m_begin;
				return *this;
			}

			Range& move_back( Iterator start ) {
				assert( m_begin > start );
				--m_begin;
				return *this;
			}

			bool at_end( ) const {
				return !(m_begin != m_end);
			}

			bool empty( ) const {
				return !(m_begin != m_end);
			}

			iterator begin( ) {
				return m_begin;
			}

			void advance( difference_type n ) {
				std::advance( m_begin, n );
			}

			void set_begin( iterator i ) {
				m_begin = i;
			}

			iterator end( ) {
				return m_end;
			}

			void set_end( iterator i ) {
				m_end = i;
			}

			void set( iterator First, iterator Last ) {
				m_begin = First;
				m_end = Last;
			}

			const_iterator begin( ) const {
				return m_begin;
			}

			const_iterator end( ) const {
				return m_end;
			}

			const_iterator cbegin( ) const {
				return m_begin;
			}

			const_iterator cend( ) const {
				return m_end;
			}

			reference front( ) {
				return *m_begin;
			}

			const_reference front( ) const {
				return *m_begin;
			}

			reference back( ) {
				auto it = m_begin;
				std::advance( it, size( ) -1 );
				return *it; 
			}

			const_reference back( ) const {
				auto it = m_begin;
				std::advance( it, size( ) -1 );
				return *it; 
			}

			reference operator*( ) {
				return *m_begin;
			}

			const_reference operator*( ) const {
				return *m_begin;
			}

			size_t size( ) const {
				return static_cast<size_t>( std::distance( m_begin, m_end ) );
			}

			reference operator[]( size_t pos ) {
				return *(m_begin + pos);
			}

			const_reference operator[]( size_t pos ) const {
				return *(m_begin + pos);
			}

			bool operator==( Range const & other ) const {
				return std::equal( m_begin, m_end, other.m_begin );
			}

			bool operator!=( Range const & other ) const {
				return !std::equal( m_begin, m_end, other.m_begin );
			}
		};	// class Range

		template<typename Iterator>
		Range<Iterator> make_range( Iterator first, Iterator last ) {
			return Range<Iterator> { first, last };
		}

		template<typename Container, typename std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
		Range<typename Container::iterator> make_range( Container & container ) {
			using Iterator = decltype(std::begin( container ));
			return Range<Iterator>( std::begin( container ), std::end( container ) );
		}

		template<typename Container, typename std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
		Range<typename Container::const_iterator> make_range( Container const & container ) {
			using Iterator = decltype(std::begin( container ));
			return Range<Iterator>( std::begin( container ), std::end( container ) );
		}

		template<typename Iterator>
		void safe_advance( Range<Iterator> & range, typename std::iterator_traits<Iterator>::difference_type count ) {
			assert( 0 <= count );
			if( std::distance( range.begin( ), range.end( ) )>= count ) {
				range.advance( count );
			} else {
				range.set_begin( range.end( ) );
			}
		}

		template<typename Iterator>
		bool contains( Range<Iterator> const & range, typename std::iterator_traits<Iterator>::value_type const & key ) {
			return std::find( range.begin( ), range.end( ), key ) != range.end( );
		}

		template<typename Iterator>
		bool at_end( Range<Iterator> const & range ) {
			return range.begin( ) == range.end( );
		}
	}	// namespace range
}	// namespace daw
