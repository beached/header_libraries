// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include "daw_algorithm.h"
#include "daw_traits.h"

namespace daw {

	template<typename Container>
	class circular_iterator {
		using iterator = decltype( std::begin( std::declval<Container>( ) ) );

	public:
		using difference_type = std::ptrdiff_t;
		using pointer = void;
		using value_type = std::decay_t<decltype( *std::declval<iterator>( ) )>;
		using iterator_category = std::random_access_iterator_tag;
		using reference = decltype( *( &( *std::begin( std::declval<Container>( ) ) ) ) );
		using const_reference = decltype( *( &( *std::cbegin( std::declval<Container>( ) ) ) ) );

	private:
		static_assert(
		  is_same_v<std::random_access_iterator_tag, typename std::iterator_traits<iterator>::iterator_category>,
		  "Container iterators must be randomly accessable" );

		Container *m_container;
		iterator m_iterator;

		constexpr std::ptrdiff_t get_offset( std::ptrdiff_t n ) const noexcept {
			auto const first = std::cbegin( *m_container );
			auto const sz = daw::algorithm::distance( first, std::end( *m_container ) );
			auto dist = daw::algorithm::distance( first, m_iterator );
			auto orig_dist = dist;
			dist += n;
			if( dist < 0 ) {
				dist *= -1;
				dist %= sz;
				dist = sz - dist;
			} else if( dist >= sz ) {
				dist %= sz;
			}
			return dist - orig_dist;
		}

	public:
		constexpr circular_iterator( ) noexcept = delete;

		constexpr circular_iterator( Container *ptr ) noexcept
		  : m_container{ptr}
		  , m_iterator{std::begin( *ptr )} {}

		constexpr circular_iterator( Container *ptr, iterator i ) noexcept
		  : m_container{ptr}
		  , m_iterator{std::move( i )} {}

		constexpr circular_iterator( circular_iterator const & ) = default;
		constexpr circular_iterator( circular_iterator && ) noexcept = default;
		constexpr circular_iterator &operator=( circular_iterator const & ) = default;
		constexpr circular_iterator &operator=( circular_iterator && ) noexcept = default;

		~circular_iterator( ) noexcept = default;

		constexpr circular_iterator &operator+=( std::ptrdiff_t n ) noexcept {
			daw::algorithm::advance( m_iterator, get_offset( n ) );
			return *this;
		}

		constexpr circular_iterator &operator-=( std::ptrdiff_t n ) {
			n *= -1;
			daw::algorithm::advance( m_iterator, get_offset( n ) );
			return *this;
		}

		constexpr reference operator*( ) {
			return *m_iterator;
		}

		constexpr iterator operator->( ) noexcept {
			return m_iterator;
		}

		constexpr circular_iterator &operator++( ) noexcept {
			daw::algorithm::advance( m_iterator, get_offset( 1 ) );
			return *this;
		}

		constexpr circular_iterator operator++(int)noexcept {
			auto result = circular_iterator{*this};
			daw::algorithm::advance( m_iterator, get_offset( 1 ) );
			return result;
		}

		constexpr circular_iterator &operator--( ) noexcept {
			daw::algorithm::advance( m_iterator, get_offset( -1 ) );
			return *this;
		}

		constexpr circular_iterator operator--(int)noexcept {
			auto result = circular_iterator{*this};
			daw::algorithm::advance( m_iterator, get_offset( -1 ) );
			return result;
		}

		constexpr circular_iterator operator+( std::ptrdiff_t n ) noexcept {
			auto tmp{*this};
			tmp += n;
			return tmp;
		}

		constexpr circular_iterator operator-( std::ptrdiff_t n ) noexcept {
			auto tmp{*this};
			tmp += n;
			return tmp;
		}

		constexpr friend bool operator==( circular_iterator const &lhs, circular_iterator const &rhs ) noexcept {
			return lhs.m_iterator == rhs.m_iterator;
		}

		constexpr friend bool operator!=( circular_iterator const &lhs, circular_iterator const &rhs ) noexcept {
			return lhs.m_iterator != rhs.m_iterator;
		}

		constexpr friend bool operator<( circular_iterator const &lhs, circular_iterator const &rhs ) noexcept {
			return lhs.m_iterator < rhs.m_iterator;
		}

		constexpr friend bool operator>( circular_iterator const &lhs, circular_iterator const &rhs ) noexcept {
			return lhs.m_iterator > rhs.m_iterator;
		}

		constexpr friend bool operator<=( circular_iterator const &lhs, circular_iterator const &rhs ) noexcept {
			return lhs.m_iterator <= rhs.m_iterator;
		}

		constexpr friend bool operator>=( circular_iterator const &lhs, circular_iterator const &rhs ) noexcept {
			return lhs.m_iterator >= rhs.m_iterator;
		}

		constexpr friend std::ptrdiff_t operator-( circular_iterator const &lhs, circular_iterator const &rhs ) noexcept {
			return lhs.m_iterator - rhs.m_iterator;
		}
	}; // circular_iterator

	template<typename Container>
	constexpr auto make_circular_iterator( Container &container ) noexcept {
		return circular_iterator<Container>{&container};
	}

	template<typename Container, typename Iterator>
	constexpr auto make_circular_iterator( Container &container, Iterator it ) noexcept {
		return circular_iterator<Container>{&container, std::move( it )};
	}
} // namespace daw
