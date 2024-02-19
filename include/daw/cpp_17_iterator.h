// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"

#include <iterator>

namespace daw {
	namespace cpp_17_details {
		template<typename InputIterator>
		[[nodiscard, maybe_unused]] constexpr ptrdiff_t
		distance_impl( InputIterator first, InputIterator last,
		               std::input_iterator_tag ) noexcept( noexcept( ++first ) ) {

			ptrdiff_t count = 0;
			while( first != last ) {
				++count;
				++first;
			}
			return count;
		}

		template<typename Iterator1, typename Iterator2>
		[[nodiscard, maybe_unused]] constexpr ptrdiff_t distance_impl(
		  Iterator1 first, Iterator2 last,
		  std::random_access_iterator_tag ) noexcept( noexcept( last - first ) ) {

			return last - first;
		}

		template<typename Iterator, typename Distance>
		constexpr void
		advance( Iterator &first, Distance n,
		         std::input_iterator_tag ) noexcept( noexcept( ++first ) ) {

			while( n-- > 0 ) {
				++first;
			}
		}

		template<typename Iterator, typename Distance>
		constexpr void
		advance( Iterator &first, Distance n,
		         std::output_iterator_tag ) noexcept( noexcept( ++first ) ) {

			while( n-- > 0 ) {
				++first;
			}
		}

		template<typename Iterator, typename Distance>
		constexpr void advance(
		  Iterator &first, Distance n,
		  std::bidirectional_iterator_tag ) noexcept( noexcept( ++first )
		                                                and noexcept( --first ) ) {

			if( n >= 0 ) {
				while( n-- > 0 ) {
					++first;
				}
			} else {
				while( ++n < 0 ) {
					--first;
				}
			}
		}

		template<typename Iterator, typename Distance>
		constexpr void
		advance( Iterator &first, Distance n,
		         std::random_access_iterator_tag ) noexcept( noexcept( first +=
		                                                               static_cast<
		                                                                 ptrdiff_t>(
		                                                                 n ) ) ) {
			first += static_cast<ptrdiff_t>( n );
		}
	} // namespace cpp_17_details

	/// @brief Calculate distance between iterators
	/// @tparam Iterator type of Iterator to compare
	/// @param first first iterator, must be <= second if Iterators are not
	/// RandomAccess
	/// @param second second iterator, must be >= first if Iterators are not
	/// RandomAccess
	/// @return	a ptrdiff_t of how many steps apart iterators are.  If Iterators
	/// are RandomAccess it may be <0, otherwise always greater
	template<typename Iterator>
	[[nodiscard]] constexpr ptrdiff_t
	distance( Iterator first, Iterator second ) noexcept(
	  noexcept( cpp_17_details::distance_impl(
	    first, second,
	    typename std::iterator_traits<Iterator>::iterator_category{ } ) ) ) {
		return cpp_17_details::distance_impl(
		  first, second,
		  typename std::iterator_traits<Iterator>::iterator_category{ } );
	}

	/// @brief Advance iterator n steps
	/// @tparam Iterator Type of iterator to advance
	/// @tparam Distance A type convertible to an integral type
	/// @param it iterator to advance
	/// @param n how far to move iterator
	template<typename Iterator, typename Distance>
	constexpr void advance( Iterator &it, Distance n ) {
		cpp_17_details::advance(
		  it, static_cast<ptrdiff_t>( n ),
		  typename std::iterator_traits<Iterator>::iterator_category{ } );
	}

	/// @brief Move iterator forward n steps, if n < 0 it is only defined for
	/// types that are Bidirectional
	/// @tparam Iterator Type of iterator to move forward
	/// @param it Iterator to advance
	/// @param n how far to move forward
	/// @return The resulting iterator of advancing it n steps
	template<typename Iterator>
	[[nodiscard]] constexpr Iterator next( Iterator it,
	                                       ptrdiff_t n = 1 ) noexcept {

		cpp_17_details::advance(
		  it, n, typename std::iterator_traits<Iterator>::iterator_category{ } );
		return std::move( it );
	}

	/// @brief Move iterator backward n steps, if n > 0, only defined for types
	/// that are Bidirectional
	/// @tparam Iterator Type of iterator to move backward
	/// @param it Iterator to advance
	/// @param n how far to move backward
	/// @return The resulting iterator of advancing it n steps
	template<typename Iterator>
	[[nodiscard]] constexpr Iterator prev( Iterator it,
	                                       ptrdiff_t n = 1 ) noexcept {

		cpp_17_details::advance(
		  it, -n, typename std::iterator_traits<Iterator>::iterator_category{ } );
		return std::move( it );
	}
} // namespace daw
