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
#include "../daw_move.h"
#include "../daw_traits.h"
#include "impl/daw_find_iterator_impl.h"

#include <cstddef>
#include <iterator>
#include <limits>

namespace daw {
	template<typename T>
	struct find_one_element {
		T value;
		std::size_t index;

		find_one_element *operator->( ) const && {
			return this;
		}

		find_one_element *operator->( ) && {
			return this;
		}
	};

	template<typename Iterator, typename IteratorLast, typename... Filters>
	class find_one_iterator
	  : find_iterator_impl::FilterProxy<Filters, std::is_class_v<Filters>>... {

	public:
		using iterator = Iterator;
		using value_type =
		  find_one_element<typename std::iterator_traits<Iterator>::value_type>;
		using difference_type =
		  typename std::iterator_traits<Iterator>::difference_type;
		using iterator_category =
		  typename DAW_TYPEOF( find_iterator_impl::test_iterator_category(
		    std::declval<typename std::iterator_traits<
		      Iterator>::iterator_category>( ) ) )::type;
		using reference = value_type;
		using pointer = find_one_element<reference>;

	private:
		Iterator m_first = Iterator{ };
		IteratorLast m_last = IteratorLast{ };
		std::size_t m_last_index = -std::size_t{ 1 };

	public:
		constexpr find_one_iterator( Iterator first, IteratorLast last,
		                             Filters... filts )
		  : find_iterator_impl::FilterProxy<Filters, std::is_class_v<Filters>>(
		      filts )...
		  , m_first( first )
		  , m_last( last ) {
			auto tmp = daw::algorithm::find_one_of(
			  m_first, m_last,
			  find_iterator_impl::FilterProxy<
			    Filters, std::is_class_v<Filters>>::filter( )... );
		}

		constexpr value_type operator*( ) {
			return { *m_first, m_last_index };
		}

		constexpr pointer operator->( ) {
			return { *m_first, m_last_index };
		}

		constexpr find_one_iterator &operator++( ) & {
			auto tmp = daw::algorithm::find_one_of(
			  std::next( m_first ), m_last,
			  find_iterator_impl::FilterProxy<
			    Filters, std::is_class_v<Filters>>::filter( )... );
			m_first = tmp.position;
			m_last_index = tmp.index;
			return *this;
		}

		constexpr find_one_iterator operator++( int ) & {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr find_one_iterator begin( ) const {
			return *this;
		}

		constexpr find_one_iterator end( ) const {
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

		friend constexpr bool operator==( find_one_iterator const &lhs,
		                                  find_one_iterator const &rhs ) noexcept {
			return lhs.m_first == rhs.m_first;
		}

		friend constexpr bool operator!=( find_one_iterator const &lhs,
		                                  find_one_iterator const &rhs ) noexcept {
			return lhs.m_first != rhs.m_first;
		}
	};

	template<typename Iterator, typename IteratorLast, typename Filter>
	find_one_iterator( Iterator, IteratorLast, Filter )
	  -> find_one_iterator<Iterator, IteratorLast, Filter>;
} // namespace daw
