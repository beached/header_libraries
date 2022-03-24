// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_attributes.h"

#include <compare>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace daw {
	template<typename Iterator>
	class wrap_iter {
	public:
		using iterator_type = Iterator;
		using value_type = typename std::iterator_traits<iterator_type>::value_type;
		using difference_type =
		  typename std::iterator_traits<iterator_type>::difference_type;
		using pointer = typename std::iterator_traits<iterator_type>::pointer;
		using reference = typename std::iterator_traits<iterator_type>::reference;
		using iterator_category =
		  typename std::iterator_traits<iterator_type>::iterator_category;

	private:
		iterator_type i{ };

	public:
		explicit wrap_iter( ) = default;

		explicit constexpr wrap_iter( iterator_type x ) noexcept
		  : i( x ) {}

		template<typename U>
		requires( std::is_convertible_v<U, iterator_type> and
		          not std::is_same_v<Iterator, U> ) //
		  explicit constexpr wrap_iter( wrap_iter<U> const &u ) noexcept
		  : i( u.base( ) ) {}

		constexpr reference operator*( ) const noexcept {
			return *i;
		}

		constexpr pointer operator->( ) const noexcept {
			return std::to_address( i );
		}

		constexpr wrap_iter &operator++( ) noexcept {
			++i;
			return *this;
		}

		[[nodiscard]] constexpr wrap_iter operator++( int ) &noexcept {
			auto tmp = *this;
			operator++( );
			return tmp;
		}

		constexpr wrap_iter &operator--( ) noexcept {
			--i;
			return *this;
		}

		[[nodiscard]] constexpr wrap_iter operator--( int ) &noexcept {
			auto tmp = *this;
			operator--( );
			return tmp;
		}

		constexpr wrap_iter operator+( difference_type n ) const noexcept {
			auto w = *this;
			w += n;
			return w;
		}

		constexpr wrap_iter &operator+=( difference_type n ) noexcept {
			i += n;
			return *this;
		}

		constexpr wrap_iter operator-( difference_type n ) const noexcept {
			return *this + ( -n );
		}

		constexpr wrap_iter &operator-=( difference_type n ) noexcept {
			*this += -n;
			return *this;
		}

		constexpr reference operator[]( difference_type n ) const noexcept {
			return i[n];
		}

		constexpr iterator_type base( ) const noexcept {
			return i;
		}

		template<typename Iterator1, typename Iterator2>
		friend constexpr auto
		operator<=>( wrap_iter<Iterator1> const &lhs,
		             wrap_iter<Iterator2> const &rhs ) noexcept {
			return lhs.base( ) <=> rhs.base( );
		}

		template<typename Iterator1, typename Iterator2>
		friend constexpr bool
		operator==( wrap_iter<Iterator1> const &lhs,
		            wrap_iter<Iterator2> const &rhs ) noexcept {
			return lhs.base( ) == rhs.base( );
		}

		template<typename Iterator1, class Iterator2>
		friend constexpr bool operator-( wrap_iter<Iterator1> const &x,
		                                 wrap_iter<Iterator2> const &y ) noexcept {
			return x.base( ) - y.base( );
		}

		template<typename Iterator1>
		friend constexpr wrap_iter<Iterator1>
		operator+( typename wrap_iter<Iterator1>::difference_type n,
		           wrap_iter<Iterator1> x ) noexcept {
			x += n;
			return x;
		}
	};
} // namespace daw
