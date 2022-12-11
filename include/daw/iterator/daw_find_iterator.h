// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../ciso646.h"
#include "../daw_algorithm.h"
#include "../daw_traits.h"
#include "impl/daw_find_iterator_impl.h"

#include <iterator>
#include <limits>

namespace daw {
	template<typename Iterator, typename IteratorLast, typename Filter>
	struct find_iterator
	  : private find_iterator_impl::FilterProxy<Filter, std::is_class_v<Filter>> {

		using iterator = Iterator;
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		using difference_type =
		  typename std::iterator_traits<Iterator>::difference_type;
		using iterator_category =
		  typename DAW_TYPEOF( find_iterator_impl::test_iterator_category(
		    std::declval<typename std::iterator_traits<
		      Iterator>::iterator_category>( ) ) )::type;
		using reference = typename std::iterator_traits<Iterator>::reference;
		using pointer = typename std::iterator_traits<Iterator>::pointer;

	private:
		Iterator m_first = Iterator{ };
		IteratorLast m_last = IteratorLast{ };

	public:
		constexpr find_iterator( Iterator first, Filter filt )
		  : find_iterator_impl::FilterProxy<Filter, std::is_class_v<Filter>>{ filt }
		  , m_first( first ) {

			m_first = daw::algorithm::find_if( m_first, m_last, this->filter( ) );
		}

		constexpr find_iterator( Iterator first, IteratorLast last, Filter filt )
		  : find_iterator_impl::FilterProxy<Filter, std::is_class_v<Filter>>( filt )
		  , m_first( first )
		  , m_last( last ) {

			m_first = daw::algorithm::find_if( m_first, m_last, this->filter( ) );
		}

		constexpr auto operator*( ) const {
			return *m_first;
		}

		constexpr auto operator*( ) {
			return *m_first;
		}

		constexpr auto operator->( ) const {
			return std::addressof( *m_first );
		}

		constexpr auto operator->( ) {
			return std::addressof( *m_first );
		}

		constexpr find_iterator &operator++( ) & {
			m_first = daw::algorithm::find_if( std::next( m_first ), m_last,
			                                   this->filter( ) );
			return *this;
		}

		constexpr find_iterator operator++( int ) & {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr find_iterator begin( ) const {
			return *this;
		}

		constexpr find_iterator end( ) const {
			auto result = *this;
			result.m_first = m_last;
			return result;
		}

		constexpr Iterator raw_begin( ) const {
			return m_first;
		}

		constexpr IteratorLast raw_end( ) const {
			return m_last;
		}

		friend constexpr bool operator==( find_iterator const &lhs,
		                                  find_iterator const &rhs ) noexcept {
			return lhs.m_first == rhs.m_first;
		}

		friend constexpr bool operator!=( find_iterator const &lhs,
		                                  find_iterator const &rhs ) noexcept {
			return lhs.m_first != rhs.m_first;
		}
	};

	template<typename Iterator, typename Filter>
	find_iterator( Iterator, Filter )
	  -> find_iterator<Iterator, Iterator, Filter>;

	template<typename Iterator, typename IteratorLast, typename Filter>
	find_iterator( Iterator, IteratorLast, Filter )
	  -> find_iterator<Iterator, IteratorLast, Filter>;
} // namespace daw
