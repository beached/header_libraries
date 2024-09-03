// Originally from the LLVM Project, under the Apache v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// Since modified by Darrell Wright
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_attributes.h"
#include "daw_compiler_fixups.h"

#include <compare>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace daw {
	template<typename Iterator, typename Tag = Iterator>
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
		  constexpr wrap_iter( wrap_iter<U, Tag> const &u ) noexcept
		  : i( u.base( ) ) {}

		constexpr reference operator*( ) const noexcept {
			return *i;
		}

		[[nodiscard]] constexpr pointer operator->( ) const noexcept {
			return std::to_address( i );
		}

		constexpr wrap_iter &operator++( ) noexcept {
			DAW_UNSAFE_BUFFER_FUNC_START
			++i;
			DAW_UNSAFE_BUFFER_FUNC_STOP
			return *this;
		}

		[[nodiscard]] constexpr wrap_iter operator++( int ) & noexcept {
			auto tmp = *this;
			operator++( );
			return tmp;
		}

		constexpr wrap_iter &operator--( ) noexcept {
			DAW_UNSAFE_BUFFER_FUNC_START
			--i;
			DAW_UNSAFE_BUFFER_FUNC_STOP
			return *this;
		}

		[[nodiscard]] constexpr wrap_iter operator--( int ) & noexcept {
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

		// clang-format off
		constexpr auto operator<=> ( wrap_iter const &rhs ) const noexcept {
			return base( ) <=> rhs.base( );
		}
		// clang-format on

		constexpr bool operator==( wrap_iter const &rhs ) const noexcept {
			return base( ) == rhs.base( );
		}

		constexpr bool operator!=( wrap_iter const &rhs ) const noexcept {
			return base( ) != rhs.base( );
		}
	};

	template<typename Iterator1, class Iterator2, typename Tag>
	constexpr bool operator-( wrap_iter<Iterator1, Tag> const &x,
	                          wrap_iter<Iterator2, Tag> const &y ) noexcept {
		return x.base( ) - y.base( );
	}

	template<typename Iterator1, typename Tag>
	constexpr wrap_iter<Iterator1, Tag>
	operator+( std::ptrdiff_t n, wrap_iter<Iterator1, Tag> x ) noexcept {
		x += n;
		return x;
	}
} // namespace daw
