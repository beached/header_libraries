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

		// don't want to depend on value_type being there
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
		  , m_position( static_cast<difference_type>( pos ) ) {

			daw::exception::dbg_precondition_check( pos <= m_pointer->size( ) );
		}

		constexpr indexed_iterator &operator+=( std::ptrdiff_t n ) noexcept {
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position + n ) <= m_pointer->size( ) );

			m_position += n;
			return *this;
		}

		constexpr indexed_iterator &operator-=( std::ptrdiff_t n ) noexcept {
			daw::exception::dbg_precondition_check( m_position - n >= 0 );

			m_position -= n;
			return *this;
		}

		constexpr reference operator*( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return ( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr const_reference operator*( ) const noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return ( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr pointer operator->( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return &( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr const_pointer operator->( ) const noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return &( *m_pointer )[static_cast<size_type>( m_position )];
		}

		constexpr indexed_iterator &operator++( ) noexcept {
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			++m_position;
			return *this;
		}

		constexpr indexed_iterator operator++( int ) noexcept {
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			auto result = indexed_iterator( *this );
			++m_position;
			return result;
		}

		constexpr indexed_iterator &operator--( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );

			--m_position;
			return *this;
		}

		constexpr indexed_iterator operator--( int ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );

			auto result = indexed_iterator( *this );
			--m_position;
			return result;
		}

		constexpr indexed_iterator operator+( std::ptrdiff_t n ) const noexcept {
			daw::exception::dbg_precondition_check( m_position + n >= 0 );

			auto result =
			  indexed_iterator( m_pointer, static_cast<size_t>( m_position + n ) );
			return result;
		}

		constexpr indexed_iterator operator-( std::ptrdiff_t n ) const noexcept {
			daw::exception::dbg_precondition_check( m_position - n >= 0 );

			auto result =
			  indexed_iterator( m_pointer, static_cast<size_t>( m_position - n ) );
			return result;
		}

		constexpr difference_type compare( indexed_iterator const &rhs ) const noexcept {
			daw::exception::dbg_precondition_check(
			  std::equal_to<>{}( m_pointer, rhs.m_pointer ) );
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check( rhs.m_position >= 0 );

			return m_position - rhs.m_position;
		}

		constexpr friend std::ptrdiff_t
		operator-( indexed_iterator const &lhs,
		           indexed_iterator const &rhs ) noexcept {
			daw::exception::dbg_precondition_check( lhs.m_pointer == rhs.m_pointer );
			daw::exception::dbg_precondition_check( lhs.m_position >= 0 );
			daw::exception::dbg_precondition_check( rhs.m_position >= 0 );
			daw::exception::dbg_precondition_check( lhs.m_position >=
			                                        rhs.m_position );

			return lhs.m_position - rhs.m_position;
		}
	}; // indexed_iterator

	template<typename T>
	constexpr auto ibegin( T &container ) noexcept {
		return indexed_iterator<T>( &container );
	}

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto ibegin( T && ) noexcept = delete;

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto ibegin( T const && ) noexcept = delete;

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
	constexpr auto iend( T && ) noexcept = delete;

	template<typename T, std::enable_if_t<daw::is_rvalue_reference_v<T>,
	                                      std::nullptr_t> = nullptr>
	constexpr auto iend( T const && ) noexcept = delete;

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

	template<typename T, size_t N>
	constexpr auto ibegin( T ( &container )[N] ) noexcept {
		return std::begin( container );
	}

	template<typename T, size_t N>
	constexpr auto ibegin( T const ( &container )[N] ) noexcept {
		return std::begin( container );
	}

	template<typename T, size_t N>
	constexpr auto cibegin( T const ( &container )[N] ) noexcept {
		return std::cbegin( container );
	}

	template<typename T, size_t N>
	constexpr auto iend( T ( &container )[N] ) noexcept {
		return std::end( container );
	}

	template<typename T, size_t N>
	constexpr auto iend( T const ( &container )[N] ) noexcept {
		return std::end( container );
	}

	template<typename T, size_t N>
	constexpr auto ciend( T const ( &container )[N] ) noexcept {
		return std::cend( container );
	}

	template<typename T>
	constexpr bool operator==( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<
	  typename T, typename U,
	  std::enable_if_t<!daw::is_convertible_v<T, U>, std::nullptr_t> = nullptr>
	constexpr bool operator==( indexed_iterator<T> const &,
	                           indexed_iterator<T> const & ) noexcept {
		return false;
	}

	template<typename T>
	constexpr bool operator!=( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<
	  typename T, typename U,
	  std::enable_if_t<!daw::is_convertible_v<T, U>, std::nullptr_t> = nullptr>
	constexpr bool operator!=( indexed_iterator<T> const &,
	                           indexed_iterator<T> const & ) noexcept {
		return true;
	}

	template<typename T>
	constexpr bool operator<( indexed_iterator<T> const &lhs,
	                          indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename T>
	constexpr bool operator>( indexed_iterator<T> const &lhs,
	                          indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename T>
	constexpr bool operator<=( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename T>
	constexpr bool operator>=( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}
} // namespace daw
