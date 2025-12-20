// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/cpp_20.h"
#include "daw/daw_move.h"

#include <daw/stdinc/iterator_traits.h>

namespace daw {
	template<typename Iterator>
	struct reverse_iterator {
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		using difference_type =
		  typename std::iterator_traits<Iterator>::difference_type;
		using size_type = size_t;
		using pointer = typename std::iterator_traits<Iterator>::pointer;
		using reference = typename std::iterator_traits<Iterator>::reference;
		using iterator_category =
		  typename std::iterator_traits<Iterator>::iterator_category;
		using i_am_a_daw_reverse_iterator = void;
	private:
		Iterator m_base{ };

	public:
		explicit reverse_iterator( ) = default;

		explicit constexpr reverse_iterator( Iterator it )
		  : m_base( std::move( it ) ) {}

		constexpr Iterator base( ) const {
			Iterator it = m_base;
			return it;
		}

		constexpr Iterator base( ) {
			Iterator it = m_base;
			return it;
		}

		constexpr decltype( auto ) operator*( ) {
			auto tmp = m_base;
			return *( --tmp );
		}

		constexpr decltype( auto ) operator*( ) const {
			auto tmp = m_base;
			return *( --tmp );
		}

		constexpr auto operator->( ) {
			auto tmp = m_base;
			return daw::to_address( ( --tmp ) );
		}

		constexpr auto operator->( ) const {
			auto tmp = m_base;
			return daw::to_address( ( --tmp ) );
		}

		constexpr decltype( auto ) operator[]( difference_type n ) {
			return *( *this + n );
		}

		constexpr decltype( auto ) operator[]( difference_type n ) const {
			return *( *this + n );
		}

		constexpr reverse_iterator &operator++( ) noexcept {
			--m_base;
			return *this;
		}

		constexpr reverse_iterator operator++( int ) noexcept {
			reverse_iterator tmp{ *this };
			--m_base;
			return tmp;
		}

		constexpr reverse_iterator &operator--( ) noexcept {
			++m_base;
			return *this;
		}

		constexpr reverse_iterator operator--( int ) noexcept {
			reverse_iterator tmp{ *this };
			++m_base;
			return tmp;
		}

		constexpr reverse_iterator operator+( difference_type n ) const {
			return reverse_iterator{ m_base - n };
		}

		constexpr reverse_iterator operator-( difference_type n ) const {
			return reverse_iterator{ m_base + n };
		}

		constexpr reverse_iterator &operator+=( difference_type n ) {
			m_base -= n;
			return *this;
		}

		constexpr reverse_iterator &operator-=( difference_type n ) {
			m_base += n;
			return *this;
		}
	}; // reverse_iterator
	template<typename Iterator>
	reverse_iterator( Iterator ) -> reverse_iterator<Iterator>;

	template<typename Iterator>
	constexpr reverse_iterator<Iterator>
	operator+( typename reverse_iterator<Iterator>::difference_type n,
	           reverse_iterator<Iterator> const &it ) {

		return reverse_iterator<Iterator>{ it.base( ) - n };
	}

	template<typename Iterator>
	constexpr reverse_iterator<Iterator>
	operator-( typename reverse_iterator<Iterator>::difference_type n,
	           reverse_iterator<Iterator> const &it ) {

		return reverse_iterator<Iterator>{ it.base( ) + n };
	}

	template<class Iterator1, class Iterator2>
	constexpr auto operator-( reverse_iterator<Iterator1> const &lhs,
	                          reverse_iterator<Iterator2> const &rhs )
	  -> decltype( rhs.base( ) - lhs.base( ) ) {

		return rhs.base( ) - lhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator==( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) == rhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator!=( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return lhs.base( ) != rhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator<( reverse_iterator<Iterator1> const &lhs,
	                          reverse_iterator<Iterator2> const &rhs ) {
		return rhs.base( ) < lhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator<=( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return rhs.base( ) <= lhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator>( reverse_iterator<Iterator1> const &lhs,
	                          reverse_iterator<Iterator2> const &rhs ) {
		return rhs.base( ) > lhs.base( );
	}

	template<typename Iterator1, typename Iterator2>
	constexpr bool operator>=( reverse_iterator<Iterator1> const &lhs,
	                           reverse_iterator<Iterator2> const &rhs ) {
		return rhs.base( ) >= lhs.base( );
	}

	template<typename Iterator>
	constexpr reverse_iterator<Iterator> make_reverse_iterator( Iterator i ) {
		return reverse_iterator<Iterator>{ std::move( i ) };
	}
} // namespace daw
