// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_assume.h"
#include "daw_attributes.h"

#include <cstddef>
#include <daw/stdinc/compare_fn.h>
#include <daw/stdinc/hash.h>
#include <daw/stdinc/iterator_traits.h>
#include <exception>
#include <type_traits>

namespace daw {
	struct never_null_t {};
	inline constexpr never_null_t never_null = never_null_t{ };

	template<typename Pointer>
	inline constexpr Pointer not_null_null_value = Pointer{ };

	template<typename Pointer>
	struct not_null_value_type {
		using type = std::remove_reference_t<decltype( *std::declval<Pointer>( ) )>;
	};

	template<typename Pointer>
	using not_null_value_type_t = typename not_null_value_type<Pointer>::type;

	template<typename Pointer>
	using not_null_value_reference_t =
	  std::conditional_t<std::is_pointer_v<Pointer>,
	                     std::remove_reference_t<Pointer>,
	                     not_null_value_type_t<Pointer> &>;

	template<typename Pointer>
	using not_null_value_const_reference_t =
	  std::conditional_t<std::is_pointer_v<Pointer>,
	                     std::remove_reference_t<Pointer>,
	                     not_null_value_type_t<Pointer> const &>;

	template<typename Pointer>
	using not_null_pointer_reference_t =
	  std::conditional_t<std::is_pointer_v<Pointer>, Pointer, Pointer &>;

	template<typename Pointer>
	using not_null_pointer_const_reference_t =
	  std::conditional_t<std::is_pointer_v<Pointer>, Pointer, Pointer const &>;

	template<class Pointer>
	struct not_null {
		using pointer = Pointer;
		using value_type = not_null_value_type_t<pointer>;
		using reference = not_null_value_reference_t<pointer>;
		using const_reference = not_null_value_const_reference_t<pointer>;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using iterator_category = std::random_access_iterator_tag;
		using pointer_reference = not_null_pointer_reference_t<pointer>;
		using pointer_const_reference = not_null_pointer_const_reference_t<pointer>;
		static constexpr pointer null_value = not_null_null_value<pointer>;

	private:
		pointer m_ptr;

	public:
		DAW_ATTRIB_INLINE constexpr not_null( pointer_const_reference ptr ) noexcept
		  : m_ptr( ptr ) {
			if( ptr == null_value ) {
				std::terminate( );
			}
		}

		DAW_ATTRIB_INLINE constexpr not_null &
		operator=( pointer_const_reference ptr ) noexcept {
			m_ptr = ptr;
			if( ptr == null_value ) {
				std::terminate( );
			}
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr not_null( never_null_t, pointer ptr ) noexcept
		  : m_ptr( ptr ) {
			DAW_ASSUME( m_ptr != null_value );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer_reference
		get( ) const noexcept {
			DAW_ASSUME( m_ptr != null_value );
			return m_ptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer_const_reference
		get( ) noexcept {
			DAW_ASSUME( m_ptr != null_value );
			return m_ptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator*( ) const noexcept {
			return *get( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) noexcept {
			return *get( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr
		operator pointer_const_reference( ) const noexcept {
			return get( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr
		operator pointer_reference( ) noexcept {
			return get( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer_const_reference
		operator->( ) const noexcept {
			return get( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer_reference
		operator->( ) noexcept {
			return get( );
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
			DAW_ASSUME( m_ptr != null_value );
			++m_ptr;
			return *this;
		}

		/// @pre (std::intptr_t)get( ) - 1 > 0
		DAW_ATTRIB_INLINE constexpr not_null &operator--( ) noexcept {
			DAW_ASSUME( m_ptr != null_value );
			--m_ptr;
			// For the rare chance that (uintptr_t)m_ptr == 1
			assert( m_ptr != null_value );
			return *this;
		}

		/// @pre (std::intptr_t)get( ) != std::numeric_limits<std::intptr_t>::max( )
		DAW_ATTRIB_INLINE constexpr not_null operator++( int ) noexcept {
			DAW_ASSUME( m_ptr != null_value );
			auto result = *this;
			++m_ptr;
			return result;
		}

		/// @pre (std::intptr_t)get( ) - 1 > 0
		DAW_ATTRIB_INLINE constexpr not_null operator--( int ) noexcept {
			DAW_ASSUME( m_ptr != null_value );
			auto result = *this;
			--m_ptr;
			// For the rare chance that (uintptr_t)m_ptr == 1
			assert( m_ptr != null_value );
			return result;
		}

		/// @pre (std::intptr_t)get( ) < std::numeric_limits<std::intptr_t>::max( )
		/// - n
		/// @pre not_null refers to a valid range of objects
		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<Integer>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr not_null &operator+=( Integer n ) noexcept {
			DAW_ASSUME( m_ptr != null_value );
			m_ptr += static_cast<difference_type>( n );
			// For the rare chance that (uintptr_t)m_ptr == -n
			assert( m_ptr != null_value );
			return *this;
		}

		/// @pre (std::intptr_t)get( ) - n > 0
		/// @pre not_null refers to a valid range of objects
		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<Integer>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr not_null &operator-=( Integer n ) noexcept {
			DAW_ASSUME( m_ptr != null_value );
			m_ptr -= static_cast<difference_type>( n );
			// For the rare chance that (uintptr_t)m_ptr == n
			assert( m_ptr != null_value );
			return *this;
		}

		/// @pre not_null refers to a valid range of objects
		template<
		  typename Integer,
		  std::enable_if_t<std::is_integral_v<Integer>, std::nullptr_t> = nullptr>
		DAW_ATTRIB_INLINE constexpr reference
		operator[]( Integer idx ) const noexcept {
			return *( get( ) + static_cast<difference_type>( idx ) );
		}

		template<typename NotNull,
		         std::enable_if_t<
		           // prevent implicit conversions
		           std::is_same_v<NotNull, not_null>,
		           std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( NotNull const &rhs ) const noexcept {
			return get( ) - rhs.get( );
		}

		/// @pre (std::intptr_t)lhs.get( ) <
		/// std::numeric_limits<std::intptr_t>::max( ) - n
		template<typename NotNull,
		         std::enable_if_t<
		           // prevent implicit conversions
		           std::is_same_v<NotNull, not_null>,
		           std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr not_null<Pointer>
		operator-( NotNull lhs, std::ptrdiff_t n ) noexcept {
			DAW_ASSUME( lhs.m_ptr != null_value );
			lhs -= n;
			return lhs;
		}

		/// @pre (std::intptr_t)lhs.get( ) + n <=
		/// std::numeric_limits<std::intptr_t>::max( )
		template<typename NotNull,
		         std::enable_if_t<
		           // prevent implicit conversions
		           std::is_same_v<NotNull, not_null>,
		           std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr not_null<Pointer>
		operator+( NotNull lhs, std::ptrdiff_t n ) noexcept {
			DAW_ASSUME( lhs.m_ptr != null_value );
			lhs += n;
			return lhs;
		}

		/// @pre (std::intptr_t)lhs.get( ) + n <=
		/// std::numeric_limits<std::intptr_t>::max( )
		template<typename NotNull,
		         std::enable_if_t<
		           // prevent implicit conversions
		           std::is_same_v<NotNull, not_null>,
		           std::nullptr_t> = nullptr>
		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr not_null<Pointer>
		operator+( std::ptrdiff_t n, NotNull rhs ) noexcept {
			DAW_ASSUME( rhs.m_ptr != null_value );
			rhs += n;
			return rhs;
		}
	};

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator==( daw::not_null<Lhs> const &lhs,
	            daw::not_null<Rhs> const &rhs ) noexcept {
		return std::equal_to<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator==( daw::not_null<Pointer> const &lhs, Pointer const &rhs ) noexcept {
		return std::equal_to<>{ }( lhs.get( ), rhs );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator==( Pointer const &lhs, daw::not_null<Pointer> const &rhs ) noexcept {
		return std::equal_to<>{ }( lhs, rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator!=( daw::not_null<Lhs> const &lhs,
	            daw::not_null<Rhs> const &rhs ) noexcept {
		return std::not_equal_to<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator!=( daw::not_null<Pointer> const &lhs, Pointer const &rhs ) noexcept {
		return std::not_equal_to<>{ }( lhs.get( ), rhs );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator!=( Pointer const &lhs, daw::not_null<Pointer> const &rhs ) noexcept {
		return std::not_equal_to<>{ }( lhs, rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<( daw::not_null<Lhs> const &lhs,
	           daw::not_null<Rhs> const &rhs ) noexcept {
		return std::less<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<( daw::not_null<Pointer> const &lhs, Pointer const &rhs ) noexcept {
		return std::less<>{ }( lhs.get( ), rhs );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<( Pointer const &lhs, daw::not_null<Pointer> const &rhs ) noexcept {
		return std::less<>{ }( lhs, rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<=( daw::not_null<Lhs> const &lhs,
	            daw::not_null<Rhs> const &rhs ) noexcept {
		return std::less_equal<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<=( daw::not_null<Pointer> const &lhs, Pointer const &rhs ) noexcept {
		return std::less_equal<>{ }( lhs.get( ), rhs );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator<=( Pointer const &lhs, daw::not_null<Pointer> const &rhs ) noexcept {
		return std::less_equal<>{ }( lhs, rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>( daw::not_null<Lhs> const &lhs,
	           daw::not_null<Rhs> const &rhs ) noexcept {
		return std::greater<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>( daw::not_null<Pointer> const &lhs, Pointer const &rhs ) noexcept {
		return std::greater<>{ }( lhs.get( ), rhs );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>( Pointer const &lhs, daw::not_null<Pointer> const &rhs ) noexcept {
		return std::greater<>{ }( lhs, rhs.get( ) );
	}

	template<typename Lhs, typename Rhs>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>=( daw::not_null<Lhs> const &lhs,
	            daw::not_null<Rhs> const &rhs ) noexcept {
		return std::greater_equal<>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>=( daw::not_null<Pointer> const &lhs, Pointer const &rhs ) noexcept {
		return std::greater_equal<>{ }( lhs.get( ), rhs );
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
	operator>=( Pointer const &lhs, daw::not_null<Pointer> const &rhs ) noexcept {
		return std::greater_equal<>{ }( lhs, rhs.get( ) );
	}

	template<typename Pointer>
	not_null( Pointer ) -> not_null<Pointer>;

	template<typename Pointer>
	not_null( never_null_t, Pointer ) -> not_null<Pointer>;

	template<typename Pointer>
	not_null( not_null<Pointer> ) -> not_null<Pointer>;

	template<typename Pointer>
	not_null( never_null_t, not_null<Pointer> ) -> not_null<Pointer>;
} // namespace daw

template<typename Pointer>
struct std::hash<daw::not_null<Pointer>> {
	[[nodiscard]] auto operator( )( daw::not_null<Pointer> value ) const
	  -> decltype( std::hash<decltype( std::declval<Pointer>( ) )>{ }(
	    value.get( ) ) ) {
		return std::hash<decltype( std::declval<Pointer>( ) )>{ }( value.get( ) );
	}
};
