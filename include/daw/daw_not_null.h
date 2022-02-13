// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_assume.h"
#include "daw_attributes.h"

#include <ciso646>
#include <cstddef>
#include <exception>
#include <functional>
#include <iterator>
#include <type_traits>

namespace daw {
	struct never_null_t {};
	inline constexpr never_null_t never_null = never_null_t{ };

	template<class Pointer>
	struct not_null {
		using pointer = Pointer;
		using value_type = std::remove_pointer_t<Pointer>;
		using reference = value_type &;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using iterator_category = std::random_access_iterator_tag;

	private:
		pointer m_ptr;

	public:
		DAW_ATTRIB_INLINE constexpr not_null( pointer ptr ) noexcept
		  : m_ptr( ptr ) {
			if( not ptr ) {
				std::terminate( );
			}
		}

		DAW_ATTRIB_INLINE constexpr not_null( never_null_t, pointer ptr ) noexcept
		  : m_ptr( ptr ) {
			DAW_ASSUME( m_ptr != nullptr );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer get( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference
		operator*( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return *m_ptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr
		operator pointer( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer
		operator->( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE explicit constexpr
		operator bool( ) const noexcept {
			return true;
		}

		// prevents compilation when someone attempts to assign a null pointer
		// constant
		not_null( std::nullptr_t ) = delete;
		not_null &operator=( std::nullptr_t ) = delete;

		/// @pre (std::intptr_t)get( ) != std::numeric_limits<std::intptr_t>::max( )
		DAW_ATTRIB_INLINE constexpr not_null &operator++( ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			++m_ptr;
			return *this;
		}

		/// @pre (std::intptr_t)get( ) - 1 > 0
		DAW_ATTRIB_INLINE constexpr not_null &operator--( ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			--m_ptr;
			return *this;
		}

		/// @pre (std::intptr_t)get( ) != std::numeric_limits<std::intptr_t>::max( )
		DAW_ATTRIB_INLINE constexpr not_null operator++( int ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			auto result = *this;
			++m_ptr;
			return result;
		}

		/// @pre (std::intptr_t)get( ) - 1 > 0
		DAW_ATTRIB_INLINE constexpr not_null operator--( int ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			auto result = *this;
			--m_ptr;
			return result;
		}

		/// @pre (std::intptr_t)get( ) < std::numeric_limits<std::intptr_t>::max( )
		/// - n
		/// @pre not_null refers to a valid range of objects
		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr not_null &operator+=( Integer n ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			m_ptr += static_cast<difference_type>( n );
			return *this;
		}

		/// @pre (std::intptr_t)get( ) - n > 0
		/// @pre not_null refers to a valid range of objects
		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr not_null &operator-=( Integer n ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			m_ptr -= static_cast<difference_type>( n );
			return *this;
		}

		/// @pre not_null refers to a valid range of objects
		template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
		                                            std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr reference
		operator[]( Integer idx ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return *( m_ptr + static_cast<difference_type>( idx ) );
		}

		template<typename NotNull,
		         std::enable_if_t<std::is_same_v<NotNull, not_null>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( NotNull const &rhs ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr - rhs.m_ptr;
		}

		/// @pre (std::intptr_t)lhs.get( ) <
		/// std::numeric_limits<std::intptr_t>::max( ) - n
		template<typename NotNull,
		         std::enable_if_t<std::is_same_v<NotNull, not_null>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr not_null<Pointer>
		operator-( NotNull lhs, std::ptrdiff_t n ) noexcept {
			DAW_ASSUME( lhs.m_ptr != nullptr );
			lhs -= n;
			return lhs;
		}

		/// @pre (std::intptr_t)lhs.get( ) + n <=
		/// std::numeric_limits<std::intptr_t>::max( )
		template<typename NotNull,
		         std::enable_if_t<std::is_same_v<NotNull, not_null>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr not_null<Pointer>
		operator+( NotNull lhs, std::ptrdiff_t n ) noexcept {
			DAW_ASSUME( lhs.m_ptr != nullptr );
			lhs += n;
			return lhs;
		}

		/// @pre (std::intptr_t)lhs.get( ) + n <=
		/// std::numeric_limits<std::intptr_t>::max( )
		template<typename NotNull,
		         std::enable_if_t<std::is_same_v<NotNull, not_null>,
		                          std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr not_null<Pointer>
		operator+( std::ptrdiff_t n, NotNull rhs ) noexcept {
			DAW_ASSUME( rhs.m_ptr != nullptr );
			rhs += n;
			return rhs;
		}
	};
	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator==( daw::not_null<Lhs *> const &lhs,
	            daw::not_null<Rhs *> const &rhs ) noexcept {
		return std::equal_to<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator!=( daw::not_null<Lhs *> const &lhs,
	            daw::not_null<Rhs *> const &rhs ) noexcept {
		return std::not_equal_to<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<( daw::not_null<Lhs *> const &lhs,
	           daw::not_null<Rhs *> const &rhs ) noexcept {
		return std::less<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<=( daw::not_null<Lhs *> const &lhs,
	            daw::not_null<Rhs *> const &rhs ) noexcept {
		return std::less_equal<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>( daw::not_null<Lhs *> const &lhs,
	           daw::not_null<Rhs *> const &rhs ) noexcept {
		return std::greater<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>=( daw::not_null<Lhs *> const &lhs,
	            daw::not_null<Rhs *> const &rhs ) noexcept {
		return std::greater_equal<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Pointer>
	not_null( Pointer ) -> not_null<Pointer>;

	template<typename Pointer>
	not_null( never_null_t, Pointer ) -> not_null<Pointer>;
} // namespace daw

template<typename Pointer>
struct std::hash<daw::not_null<Pointer>> {
	[[nodiscard]] auto operator( )( daw::not_null<Pointer> value ) const
	  -> decltype(
	    std::hash<decltype( std::declval<Pointer>( ) )>{ }( value.get( ) ) ) {
		return std::hash<decltype( std::declval<Pointer>( ) )>{ }( value.get( ) );
	}
};
