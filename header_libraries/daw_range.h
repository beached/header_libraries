#pragma once
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


#include <iterator>

#include "daw_traits.h"

namespace daw {
	namespace range {
		template<typename Iterator>
		struct Range {
			Iterator first;
			Iterator last;
			using value_type = typename std::iterator_traits<Iterator>::value_type;
			using reference = typename std::iterator_traits<Iterator>::reference;
			using const_reference = const reference;
			using iterator = Iterator;
			using const_iterator = const iterator;

			Range( ) :
				first( nullptr ),
				last( nullptr ) { }

			Range( Iterator First, Iterator Last ) :
				first( First ),
				last( Last ) { }

			Range& move_next( ) {
				assert( first != last );
				++first;
				return *this;
			}

			Range& move_back( ) {
				--first;
				return *this;
			}

			Range& move_back( Iterator start ) {
				assert( first > start );
				--first;
				return *this;
			}

			bool at_end( ) const {
				return first == last;
			}

			bool empty( ) const {
				return first == last;
			}

			iterator begin( ) {
				return first;
			}

			iterator end( ) {
				return last;
			}

			const_iterator begin( ) const {
				return first;
			}

			const_iterator end( ) const {
				return last;
			}

			const_iterator cbegin( ) const {
				return first;
			}

			const_iterator cend( ) const {
				return last;
			}

			reference front( ) {
				return *first;
			}

			const_reference front( ) const {
				return *first;
			}

			size_t size( ) const {
				return std::distance( first, last );
			}
		};	// struct Range

		template<typename Iterator>
		Range<Iterator> make_range( Iterator first, Iterator last ) {
			assert( std::distance( first, last ) >= 0 );
			return Range < Iterator > { first, last };
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
			if( std::distance( range.first, range.last ) >= count ) {
				range.first += count;
			} else {
				range.first = range.last;
			}
		}

		template<typename Iterator>
		bool contains( Range<Iterator> const & range, typename std::iterator_traits<Iterator>::value_type const & key ) {
			return std::find( range.first, range.last, key ) != range.last;
		}

		template<typename Iterator>
		bool at_end( Range<Iterator> const & range ) {
			return range.first == range.last;
		}
	}	// namespace range
}	// namespace daw
