// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <iterator>

#include "../daw_exception.h"
#include "../daw_traits.h"

namespace daw {
	template<typename T>
	struct indexed_iterator {
		using difference_type = std::ptrdiff_t;

		// using decltype should allow for c arrays
		using value_type = std::remove_reference_t<decltype(
		  std::declval<T>( )[std::declval<size_t>( )] )>;

		using pointer = value_type *;
		using const_pointer = std::remove_const_t<value_type> const *;
		using iterator_category = std::random_access_iterator_tag;
		using reference = value_type &;
		using const_reference = std::remove_const_t<value_type> const &;
		using size_type = size_t;

	private:
		T *m_pointer;
		difference_type m_position;

	public:
		explicit constexpr indexed_iterator( T *container, size_t pos = 0 ) noexcept
		  : m_pointer( container )
		  , m_position( static_cast<difference_type>( pos ) ) {}

		constexpr indexed_iterator &operator+=( std::ptrdiff_t n ) noexcept {
			m_position += n;
			return *this;
		}

		constexpr indexed_iterator &operator-=( std::ptrdiff_t n ) noexcept {
			m_position -= n;
			return *this;
		}

		constexpr reference operator*( ) noexcept {
			daw::exception::dbg_precondition_check(
			  0 <= m_position &&
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );
			return ( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr const_reference operator*( ) const noexcept {
			daw::exception::dbg_precondition_check(
			  0 <= m_position &&
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );
			return ( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr pointer operator->( ) noexcept {
			daw::exception::dbg_precondition_check(
			  0 <= m_position &&
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );
			return &( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr const_pointer operator->( ) const noexcept {
			daw::exception::dbg_precondition_check(
			  0 <= m_position &&
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );
			return &( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr indexed_iterator &operator++( ) noexcept {
			++m_position;
			return *this;
		}

		constexpr indexed_iterator operator++( int ) noexcept {
			auto result = indexed_iterator( *this );
			++m_position;
			return result;
		}

		constexpr indexed_iterator &operator--( ) noexcept {
			--m_position;
			return *this;
		}

		constexpr indexed_iterator operator--( int ) noexcept {
			auto result = indexed_iterator( *this );
			--m_position;
			return result;
		}

		constexpr indexed_iterator operator+( difference_type n ) noexcept {
			auto result = indexed_iterator( *this );
			result.m_position += n;
			return result;
		}

		constexpr indexed_iterator operator-( difference_type n ) noexcept {
			auto result = indexed_iterator( *this );
			result.m_position -= n;
			return result;
		}

		constexpr friend bool operator==( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) ==
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator!=( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) !=
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator<( indexed_iterator const &lhs,
		                                 indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) <
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator>( indexed_iterator const &lhs,
		                                 indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) >
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator<=( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) <=
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator>=( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) >=
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend difference_type
		operator-( indexed_iterator const &lhs,
		           indexed_iterator const &rhs ) noexcept {
			daw::exception::dbg_precondition_check( lhs.m_pointer == rhs.m_pointer );
			return lhs.m_position - rhs.m_position;
		}
	}; // indexed_iterator

	template<typename T>
	constexpr auto ibegin( T &container ) noexcept {
		return indexed_iterator<T>( &container );
	}

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto ibegin( T &&container ) noexcept = delete;

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto ibegin( T const &&container ) noexcept = delete;

	template<typename T>
	constexpr auto ibegin( T const &container ) noexcept {
		return indexed_iterator<T const>( &container );
	}

	template<typename T>
	constexpr auto cibegin( T const &container ) noexcept {
		return indexed_iterator<T const>( &container );
	}

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto cibegin( T &&container ) noexcept = delete;

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto cibegin( T const &&container ) noexcept = delete;

	template<typename T>
	constexpr auto iend( T &container ) noexcept {
		return indexed_iterator<T>( &container, container.size( ) );
	}

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto iend( T &&container ) noexcept = delete;

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto iend( T const &&container ) noexcept = delete;

	template<typename T>
	constexpr auto iend( T const &container ) noexcept {
		return indexed_iterator<T const>( &container, container.size( ) );
	}

	template<typename T>
	constexpr auto ciend( T const &container ) noexcept {
		return indexed_iterator<T const>( &container, container.size( ) );
	}

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto ciend( T &&container ) noexcept = delete;

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto ciend( T const &&container ) noexcept = delete;

	template<typename T>
	struct indexed_iterator<T *> {
		using difference_type = std::ptrdiff_t;

		// using decltype should allow for c arrays
		using value_type = std::remove_reference_t<decltype(
		  std::declval<T *>( )[std::declval<size_t>( )] )>;

		using pointer = value_type *;
		using const_pointer = std::remove_const_t<value_type> const *;
		using iterator_category = std::random_access_iterator_tag;
		using reference = value_type &;
		using const_reference = std::remove_const_t<value_type> const &;
		using size_type = size_t;

	private:
		T *m_pointer;
		difference_type m_position;

	public:
		explicit constexpr indexed_iterator( T *container, size_t pos = 0 ) noexcept
		  : m_pointer( container )
		  , m_position( static_cast<difference_type>( pos ) ) {}

		constexpr indexed_iterator &operator+=( std::ptrdiff_t n ) noexcept {
			m_position += n;
			return *this;
		}

		constexpr indexed_iterator &operator-=( std::ptrdiff_t n ) noexcept {
			m_position -= n;
			return *this;
		}

		constexpr reference operator*( ) noexcept {
			daw::exception::dbg_precondition_check( 0 <= m_position );
			return m_pointer[static_cast<size_type>( m_position )];
		}

		constexpr const_reference operator*( ) const noexcept {
			daw::exception::dbg_precondition_check( 0 <= m_position );
			return m_pointer[static_cast<size_type>( m_position )];
		}

		constexpr pointer operator->( ) noexcept {
			daw::exception::dbg_precondition_check( 0 <= m_position );
			return &m_pointer[static_cast<size_type>( m_position )];
		}

		constexpr const_pointer operator->( ) const noexcept {
			daw::exception::dbg_precondition_check( 0 <= m_position );
			return &m_pointer[static_cast<size_type>( m_position )];
		}

		constexpr indexed_iterator &operator++( ) noexcept {
			++m_position;
			return *this;
		}

		constexpr indexed_iterator operator++( int ) noexcept {
			auto result = indexed_iterator( *this );
			++m_position;
			return result;
		}

		constexpr indexed_iterator &operator--( ) noexcept {
			--m_position;
			return *this;
		}

		constexpr indexed_iterator operator--( int ) noexcept {
			auto result = indexed_iterator( *this );
			--m_position;
			return result;
		}

		constexpr indexed_iterator operator+( difference_type n ) noexcept {
			auto result = indexed_iterator( *this );
			result.m_position += n;
			return result;
		}

		constexpr indexed_iterator operator-( difference_type n ) noexcept {
			auto result = indexed_iterator( *this );
			result.m_position -= n;
			return result;
		}

		constexpr friend bool operator==( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) ==
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator!=( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) !=
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator<( indexed_iterator const &lhs,
		                                 indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) <
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator>( indexed_iterator const &lhs,
		                                 indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) >
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator<=( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) <=
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend bool operator>=( indexed_iterator const &lhs,
		                                  indexed_iterator const &rhs ) noexcept {
			return std::tie( lhs.m_pointer, lhs.m_position ) >=
			       std::tie( rhs.m_pointer, rhs.m_position );
		}

		constexpr friend difference_type
		operator-( indexed_iterator const &lhs,
		           indexed_iterator const &rhs ) noexcept {
			daw::exception::dbg_precondition_check( lhs.m_pointer == rhs.m_pointer );
			return lhs.m_position - rhs.m_position;
		}
	}; // indexed_iterator<T*>

	template<typename T>
	constexpr auto ibegin( T *container ) noexcept {
		return indexed_iterator<T *>( container );
	}

	template<typename T>
	constexpr auto ibegin( T const *container ) noexcept {
		return indexed_iterator<T const *>( container );
	}

	template<typename T>
	constexpr auto cibegin( T const *container ) noexcept {
		return indexed_iterator<T const *>( container );
	}

	template<typename T>
	constexpr auto iend( T *container, size_t container_size ) noexcept {
		return indexed_iterator<T *>( container, container_size );
	}

	template<typename T, size_t N>
	constexpr auto iend( T ( &container )[N] ) noexcept {
		return indexed_iterator<T *>( container, N );
	}

	template<typename T>
	constexpr auto iend( T const *container, size_t container_size ) noexcept {
		return indexed_iterator<T const *>( container, container_size );
	}

	template<typename T, size_t N>
	constexpr auto iend( T const ( &container )[N] ) noexcept {
		return indexed_iterator<T const *>( container, N );
	}

	template<typename T>
	constexpr auto ciend( T const *container, size_t container_size ) noexcept {
		return indexed_iterator<T const *>( container, container_size );
	}

	template<typename T, size_t N>
	constexpr auto ciend( T const ( &container )[N] ) noexcept {
		return indexed_iterator<T const *>( container, N );
	}

} // namespace daw
