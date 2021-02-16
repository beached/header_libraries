// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../daw_container_traits.h"
#include "../daw_exception.h"
#include "../daw_traits.h"

#include <ciso646>
#include <cstddef>
#include <iterator>

namespace daw {
	template<typename T>
	struct indexed_iterator {
		using container_type = T;
		using value_type = typename container_traits<T>::value_type;
		using reference = decltype( std::declval<T>( )[0U] );
		using const_reference = typename container_traits<T>::const_reference;
		using size_type = typename container_traits<T>::size_type;
		using difference_type = typename container_traits<T>::difference_type;
		using pointer = std::add_pointer_t<std::remove_reference_t<reference>>;
		using const_pointer =
		  std::add_pointer_t<std::remove_reference_t<const_reference>>;
		using iterator_category = std::random_access_iterator_tag;

	private:
		T *m_pointer;
		difference_type m_position;

	public:
		explicit constexpr indexed_iterator( T *container, size_t pos ) noexcept
		  : m_pointer( container )
		  , m_position( static_cast<difference_type>( pos ) ) {

			daw::exception::dbg_precondition_check( pos <= m_pointer->size( ) );
		}

		constexpr indexed_iterator &operator+=( difference_type n ) noexcept {
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position + n ) <= m_pointer->size( ) );

			m_position += n;
			return *this;
		}

		constexpr indexed_iterator &operator-=( difference_type n ) noexcept {
			daw::exception::dbg_precondition_check( m_position - n >= 0 );

			m_position -= n;
			return *this;
		}

		[[nodiscard]] constexpr reference operator*( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return m_pointer->operator[]( static_cast<size_type>( m_position ) );
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return m_pointer->operator[]( static_cast<size_type>( m_position ) );
		}

		[[nodiscard]] constexpr pointer operator->( ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return &m_pointer->operator[]( static_cast<size_type>( m_position ) );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check(
			  static_cast<size_type>( m_position ) <= m_pointer->size( ) );

			return &m_pointer->operator[]( static_cast<size_type>( m_position ) );
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

		constexpr const indexed_iterator operator--( int ) noexcept {
			daw::exception::dbg_precondition_check( m_position >= 0 );

			auto result = indexed_iterator( *this );
			--m_position;
			return result;
		}

		constexpr indexed_iterator operator+( difference_type n ) const noexcept {
			auto const new_pos = m_position + n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		[[nodiscard]] constexpr indexed_iterator
		next( difference_type n = 0 ) const noexcept {
			auto const new_pos = m_position + n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		constexpr indexed_iterator &advance( difference_type n = 0 ) noexcept {
			auto const new_pos = m_position + n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			m_position = new_pos;
			return *this;
		}

		constexpr indexed_iterator operator-( difference_type n ) const noexcept {
			auto const new_pos = m_position - n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );

			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		[[nodiscard]] constexpr indexed_iterator
		prev( difference_type n = 0 ) const noexcept {
			auto const new_pos = m_position - n;
			daw::exception::dbg_precondition_check(
			  new_pos >= 0 &&
			  new_pos < static_cast<difference_type>( m_pointer->size( ) ) );
			auto result =
			  indexed_iterator( m_pointer, static_cast<size_type>( new_pos ) );
			return result;
		}

		template<typename U>
		[[nodiscard]] constexpr difference_type
		compare( indexed_iterator<U> const &rhs ) const noexcept {
			daw::exception::dbg_precondition_check(
			  std::equal_to<>{ }( m_pointer, rhs.m_pointer ) );
			daw::exception::dbg_precondition_check( m_position >= 0 );
			daw::exception::dbg_precondition_check( rhs.m_position >= 0 );

			auto const result = m_position - rhs.m_position;
			if( result < 0 ) {
				return -1;
			}
			if( result > 0 ) {
				return 1;
			}
			return result;
		}

		constexpr friend difference_type
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
	[[nodiscard]] constexpr auto ibegin( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to ibegin will cause errors via "
		               "dangling references" );

		return indexed_iterator<std::remove_reference_t<T>>( &container, 0U );
	}

	template<typename T>
	[[nodiscard]] constexpr auto cibegin( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to cibegin will cause errors via "
		               "dangling references" );

		return indexed_iterator<daw::remove_cvref_t<T> const>( &container, 0U );
	}

	template<typename T>
	[[nodiscard]] constexpr auto iend( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to iend will cause errors via "
		               "dangling references" );

		return indexed_iterator<std::remove_reference_t<T>>( &container,
		                                                     container.size( ) );
	}

	template<typename T>
	[[nodiscard]] constexpr auto ciend( T &&container ) noexcept {
		static_assert( std::is_lvalue_reference_v<T>,
		               "Passing a temporary to ciend will cause errors via "
		               "dangling references" );

		return indexed_iterator<daw::remove_cvref_t<T> const>( &container,
		                                                       container.size( ) );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto ibegin( T ( &container )[N] ) noexcept {
		return std::begin( container );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto ibegin( T const ( &container )[N] ) noexcept {
		return std::begin( container );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto cibegin( T ( &container )[N] ) noexcept {
		return std::cbegin( container );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto cibegin( T const ( &container )[N] ) noexcept {
		return std::cbegin( container );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto iend( T ( &container )[N] ) noexcept {
		return std::end( container );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto iend( T const ( &container )[N] ) noexcept {
		return std::end( container );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto ciend( T ( &container )[N] ) noexcept {
		return std::cend( container );
	}

	template<typename T, size_t N>
	[[nodiscard]] constexpr auto ciend( T const ( &container )[N] ) noexcept {
		return std::cend( container );
	}

	template<typename T>
	constexpr bool operator==( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<
	  typename T, typename U,
	  std::enable_if_t<not std::is_convertible_v<T, U>, std::nullptr_t> = nullptr>
	constexpr bool operator==( indexed_iterator<T> const &,
	                           indexed_iterator<U> const & ) noexcept {
		return false;
	}

	template<typename T>
	constexpr bool operator!=( indexed_iterator<T> const &lhs,
	                           indexed_iterator<T> const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<
	  typename T, typename U,
	  std::enable_if_t<not std::is_convertible_v<T, U>, std::nullptr_t> = nullptr>
	constexpr bool operator!=( indexed_iterator<T> const &,
	                           indexed_iterator<U> const & ) noexcept {
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
