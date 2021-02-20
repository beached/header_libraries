// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cassert>
#include <iterator>
#include <memory>

namespace daw {
	template<typename T>
	struct pointer_iterator {
		using value_type = T;
		using reference = T &;
		using difference_type = std::ptrdiff_t;
		using pointer = T *;
		using iterator_category = std::random_access_iterator_tag;

	private:
		pointer m_ptr;

	public:
		constexpr pointer_iterator( pointer ptr )
		  : m_ptr( ptr ) {
			assert( ptr );
		}

		template<typename U,
		         std::enable_if_t<(not std::is_const_v<U> and
		                           std::is_same_v<std::remove_const_t<U>, T>),
		                          std::nullptr_t> = nullptr>
		constexpr pointer_iterator( pointer_iterator<U> other )
		  : pointer_iterator( other.m_ptr ) {}

		template<typename U,
		         std::enable_if_t<(not std::is_const_v<U> and
		                           std::is_same_v<std::remove_const_t<U>, T>),
		                          std::nullptr_t> = nullptr>
		constexpr pointer_iterator &operator=( pointer_iterator<U> rhs ) {
			m_ptr = rhs.m_ptr;
		}

		constexpr operator pointer_iterator<value_type const>( ) const {
			return {m_ptr};
		}

		constexpr pointer_iterator( std::nullptr_t ) = delete;

		constexpr pointer operator->( ) const {
			return m_ptr;
		}

		constexpr reference operator*( ) const {
			return *m_ptr;
		}

		constexpr reference operator[]( difference_type idx ) const {
			return m_ptr[idx];
		}

		constexpr pointer_iterator &operator++( ) {
			++m_ptr;
			return *this;
		}

		constexpr pointer_iterator &operator++( int ) {
			auto result = *this;
			++m_ptr;
			return result;
		}

		constexpr pointer_iterator &operator--( ) {
			--m_ptr;
			return *this;
		}

		constexpr pointer_iterator &operator--( int ) {
			auto result = *this;
			--m_ptr;
			return result;
		}

		constexpr pointer_iterator &operator+=( difference_type d ) {
			m_ptr += d;
			return *this;
		}

		constexpr pointer_iterator &operator-=( difference_type d ) {
			m_ptr -= d;
			return *this;
		}

		constexpr pointer_iterator operator+( difference_type d ) const {
			auto result = *this;
			result += d;
			return result;
		}

		constexpr pointer_iterator operator-=( difference_type d ) const {
			auto result = *this;
			result -= d;
			return result;
		}

		friend constexpr bool operator==( pointer_iterator const &lhs,
		                                  pointer_iterator const &rhs ) {
			return lhs.m_ptr == rhs.ptr;
		}

		friend constexpr bool operator!=( pointer_iterator const &lhs,
		                                  pointer_iterator const &rhs ) {
			return lhs.m_ptr != rhs.m_ptr;
		}

		friend constexpr bool operator<( pointer_iterator const &lhs,
		                                 pointer_iterator const &rhs ) {
			return lhs.m_ptr < rhs.m_ptr;
		}

		friend constexpr bool operator<=( pointer_iterator const &lhs,
		                                  pointer_iterator const &rhs ) {
			return lhs.m_ptr <= rhs.m_ptr;
		}

		friend constexpr bool operator>( pointer_iterator const &lhs,
		                                 pointer_iterator const &rhs ) {
			return lhs.m_ptr > rhs.m_ptr;
		}

		friend constexpr bool operator>=( pointer_iterator const &lhs,
		                                  pointer_iterator const &rhs ) {
			return lhs.m_ptr >= rhs.m_ptr;
		}
	};

	/*
	template<typename T>
	struct pointer_iterator<T const> {
	  using value_type = T;
	  using reference = T const &;
	  using difference_type = std::ptrdiff_t;
	  using pointer = T const *;
	  using iterator_category = std::random_access_iterator_tag;

	private:
	  pointer m_ptr;

	public:
	  constexpr pointer_iterator( pointer ptr )
	    : m_ptr( ptr ) {
	    assert( ptr );
	  }

	  constexpr pointer_iterator( std::nullptr_t ) = delete;

	  constexpr pointer operator->( ) const {
	    return m_ptr;
	  }

	  constexpr reference operator*( ) const {
	    return *m_ptr;
	  }

	  constexpr reference operator[]( difference_type idx ) const {
	    return m_ptr[idx];
	  }

	  constexpr pointer_iterator &operator++( ) {
	    ++m_ptr;
	    return *this;
	  }

	  constexpr pointer_iterator &operator++( int ) {
	    auto result = *this;
	    ++m_ptr;
	    return result;
	  }

	  constexpr pointer_iterator &operator--( ) {
	    --m_ptr;
	    return *this;
	  }

	  constexpr pointer_iterator &operator--( int ) {
	    auto result = *this;
	    --m_ptr;
	    return result;
	  }

	  constexpr pointer_iterator &operator+=( difference_type d ) {
	    m_ptr += d;
	    return *this;
	  }

	  constexpr pointer_iterator &operator-=( difference_type d ) {
	    m_ptr -= d;
	    return *this;
	  }

	  constexpr pointer_iterator operator+( difference_type d ) const {
	    auto result = *this;
	    result += d;
	    return result;
	  }

	  constexpr pointer_iterator operator-=( difference_type d ) const {
	    auto result = *this;
	    result -= d;
	    return result;
	  }

	  friend constexpr bool operator==( pointer_iterator const &lhs,
	                                    pointer_iterator const &rhs ) {
	    return lhs.m_ptr == rhs.ptr;
	  }

	  friend constexpr bool operator!=( pointer_iterator const &lhs,
	                                    pointer_iterator const &rhs ) {
	    return lhs.m_ptr != rhs.m_ptr;
	  }

	  friend constexpr bool operator<( pointer_iterator const &lhs,
	                                   pointer_iterator const &rhs ) {
	    return lhs.m_ptr < rhs.m_ptr;
	  }

	  friend constexpr bool operator<=( pointer_iterator const &lhs,
	                                    pointer_iterator const &rhs ) {
	    return lhs.m_ptr <= rhs.m_ptr;
	  }

	  friend constexpr bool operator>( pointer_iterator const &lhs,
	                                   pointer_iterator const &rhs ) {
	    return lhs.m_ptr > rhs.m_ptr;
	  }

	  friend constexpr bool operator>=( pointer_iterator const &lhs,
	                                    pointer_iterator const &rhs ) {
	    return lhs.m_ptr >= rhs.m_ptr;
	  }
};
*/
} // namespace daw
