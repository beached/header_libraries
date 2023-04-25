// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

// Experiment

#pragma once

#include "daw_utility.h"

#include <cassert>
#include <cstddef>
#include <exception>
#include <functional>
#include <iterator>
#include <span>
#include <utility>

#if not defined( DAW_NO_SAFE_POINTER_CHECKS )
#define DAW_SAFE_POINTER_ENSURE( ... ) \
	do {                                 \
		assert( ( __VA_ARGS__ ) );         \
	} while( false )
#else
#define DAW_SAFE_POINTER_ENSURE( ... )
#endif
namespace daw::memory {
	template<typename T>
	struct safe_pointer {
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using value_type = T;
		using pointer = std::add_pointer_t<value_type>;
		using iterator_category = std::random_access_iterator_tag;
		using reference = std::add_lvalue_reference_t<value_type>;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<value_type>>;

	private:
		pointer m_pointer = { };
		std::span<T> m_range = { };
		bool m_destroyed = false;

		[[nodiscard]] constexpr std::size_t pointer_index( ) const {
			if( not m_pointer ) {
				DAW_SAFE_POINTER_ENSURE( not m_range.data( ) );
				return 0;
			}
			return static_cast<std::size_t>(
			  std::distance( m_range.data( ), m_pointer ) );
		}

		[[nodiscard]] constexpr bool pointer_in_range( pointer p ) const {
			return ( not get_base( ) and not p ) or
			       ( std::less_equal<>{ }( get_base( ), p ) and
			         std::less<>{ }( p, daw::data_end( m_range ) ) );
		}

		[[nodiscard]] constexpr bool same_range( safe_pointer const &rhs ) const {
			if( get_base( ) == rhs.get_base( ) and
			    get_base_size( ) == rhs.get_base_size( ) ) {
				return true;
			}
			if( get_base( ) and not rhs.get_base( ) and rhs.get( ) ) {
				return pointer_in_range( rhs.get( ) );
			}
			return not get_base( ) and not rhs.get_base( );
		}

		[[nodiscard]] constexpr bool can_add( difference_type n ) const {
			if( n < 0 ) {
				return can_sub( -n );
			}
			return static_cast<std::size_t>( n ) <=
			       ( m_range.size( ) - pointer_index( ) );
		}

		[[nodiscard]] constexpr bool can_sub( difference_type n ) const {
			if( n < 0 ) {
				return can_add( -n );
			}
			return static_cast<std::size_t>( n ) <= pointer_index( );
		}

		template<typename>
		friend class safe_pointer;

		constexpr safe_pointer( pointer p, std::span<value_type> s ) noexcept
		  : m_pointer( p )
		  , m_range( s ) {}

	public:
		explicit safe_pointer( ) = default;

		constexpr safe_pointer( std::nullptr_t ) noexcept {}

		template<typename Pointer,
		         std::enable_if_t<std::is_pointer_v<Pointer> and
		                            std::is_constructible_v<pointer, Pointer>,
		                          std::nullptr_t> = nullptr>
		constexpr safe_pointer( Pointer base ) noexcept
		  : m_pointer( base ) {
			DAW_SAFE_POINTER_ENSURE( base );
		}

		template<std::size_t N>
		constexpr safe_pointer( value_type ( &base )[N] ) noexcept
		  : m_pointer( base )
		  , m_range( base, N ) {
			DAW_SAFE_POINTER_ENSURE( base );
		}

		explicit constexpr safe_pointer( pointer base, size_type size ) noexcept
		  : m_pointer( base )
		  , m_range( base, size ) {
			DAW_SAFE_POINTER_ENSURE( base );
		}

		[[nodiscard]] constexpr pointer get( ) const noexcept {
			return m_pointer;
		}

		[[nodiscard]] constexpr pointer get_base( ) const noexcept {
			if( m_range.data( ) ) {
				return m_range.data( );
			}
			return m_pointer;
		}

		[[nodiscard]] constexpr std::size_t get_base_size( ) const noexcept {
			return m_range.size( );
		}

		constexpr pointer destroy( ) noexcept {
			m_destroyed = true;
			pointer result = get_base( );
			DAW_SAFE_POINTER_ENSURE( result );
			return result;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const noexcept {
			return not m_destroyed and static_cast<bool>( m_pointer );
		}

		template<typename Integer,
		         std::enable_if<( std::is_integral_v<Integer> and
		                          sizeof( Integer ) == sizeof( std::uintptr_t ) ),
		                        std::nullptr_t> = nullptr>
		[[nodiscard]] inline explicit operator Integer( ) const {
			DAW_SAFE_POINTER_ENSURE( *this );
			return reinterpret_cast<Integer>( get( ) );
		}

		template<typename U, std::enable_if_t<std::is_constructible_v<U *, pointer>,
		                                      std::nullptr_t> = nullptr>
		inline explicit operator safe_pointer<U>( ) const {
			U *p = static_cast<U *>( m_pointer );
			if constexpr( std::is_same_v<void, std::remove_const_t<U>> ) {
				return safe_pointer<U>( p );
			} else {
				U *d = static_cast<U *>( m_range.data( ) );
				return safe_pointer<U>( p, std::span<U>( d, m_range.size( ) ) );
			}
		}

		[[nodiscard]] constexpr reference operator*( ) const noexcept {
			DAW_SAFE_POINTER_ENSURE( *this );
			return m_range.front( );
		}

		[[nodiscard]] constexpr pointer operator->( ) const noexcept {
			DAW_SAFE_POINTER_ENSURE( not m_destroyed );
			return get( );
		}

		constexpr safe_pointer &operator++( ) noexcept {
			DAW_SAFE_POINTER_ENSURE( *this );
			DAW_SAFE_POINTER_ENSURE( pointer_index( ) < get_base_size( ) );
			++m_pointer;
			return *this;
		}

		[[nodiscard]] constexpr safe_pointer operator++( int ) &noexcept {
			auto result = *this;
			safe_pointer::operator++( );
			return result;
		}

		constexpr safe_pointer &operator--( ) noexcept {
			DAW_SAFE_POINTER_ENSURE( *this );
			DAW_SAFE_POINTER_ENSURE( pointer_index( ) > 0 );
			--m_pointer;
			return *this;
		}

		[[nodiscard]] constexpr safe_pointer operator--( int ) &noexcept {
			auto result = *this;
			safe_pointer::operator--( );
			return result;
		}

		constexpr safe_pointer &operator+=( difference_type n ) noexcept {
			DAW_SAFE_POINTER_ENSURE( can_add( n ) );
			if( n < 0 ) {
				return safe_pointer::operator-=( -n );
			}
			m_pointer += n;
			return *this;
		}

		[[nodiscard]] constexpr safe_pointer
		operator+( difference_type n ) const noexcept {
			auto result = *this;
			result += n;
			return result;
		}

		constexpr safe_pointer &operator-=( difference_type n ) noexcept {
			DAW_SAFE_POINTER_ENSURE( can_sub( n ) );
			if( n < 0 ) {
				return safe_pointer::operator+=( -n );
			}
			m_pointer -= n;
			return *this;
		}

		[[nodiscard]] constexpr safe_pointer
		operator-( difference_type n ) const noexcept {
			auto result = *this;
			result -= n;
			return result;
		}

		template<typename U,
		         std::enable_if_t<
		           std::is_same_v<std::remove_const_t<T>, std::remove_const_t<U>>,
		           std::nullptr_t> = nullptr>
		[[nodiscard]] constexpr difference_type
		operator-( safe_pointer<U> const &rhs ) const noexcept {
			DAW_SAFE_POINTER_ENSURE( pointer_in_range( rhs.get( ) ) );
			return get( ) - rhs.get( );
		}

		[[nodiscard]] constexpr difference_type
		operator-( pointer rhs ) const noexcept {
			DAW_SAFE_POINTER_ENSURE( pointer_in_range( rhs ) );
			return get( ) - rhs;
		}

		[[nodiscard]] constexpr reference
		operator[]( difference_type n ) const noexcept {
			auto tmp = *this;
			tmp += n;
			return *tmp;
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator==( safe_pointer<U> const &rhs ) const noexcept {
			return std::equal_to<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator!=( safe_pointer<U> const &rhs ) const noexcept {
			return std::not_equal_to<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator<( safe_pointer<U> const &rhs ) const noexcept {
			return std::less<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator<=( safe_pointer<U> const &rhs ) const noexcept {
			return std::less_equal<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator>( safe_pointer<U> const &rhs ) const noexcept {
			return std::greater<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator>=( safe_pointer<U> const &rhs ) const noexcept {
			return std::greater_equal<>{ }( get( ), rhs.get( ) );
		}
	};

	template<typename T>
	requires(
	  std::is_same_v<std::remove_const_t<T>, void> ) struct safe_pointer<T> {
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using value_type = T;
		using pointer = std::add_pointer_t<value_type>;
		using iterator_category = std::random_access_iterator_tag;
		using reference = std::add_lvalue_reference_t<value_type>;
		using const_reference =
		  std::add_lvalue_reference_t<std::add_const_t<value_type>>;

	private:
		pointer m_pointer = { };
		bool m_destroyed = false;

		template<typename>
		friend class safe_pointer;

	public:
		explicit safe_pointer( ) = default;

		constexpr safe_pointer( std::nullptr_t ) noexcept {}

		template<typename Pointer,
		         std::enable_if_t<std::is_pointer_v<Pointer> and
		                            std::is_constructible_v<pointer, Pointer>,
		                          std::nullptr_t> = nullptr>
		constexpr safe_pointer( Pointer base ) noexcept
		  : m_pointer( base ) {
			DAW_SAFE_POINTER_ENSURE( base );
		}

		explicit constexpr safe_pointer( pointer base, size_type size ) noexcept
		  : m_pointer( base ) {
			(void)size;
			DAW_SAFE_POINTER_ENSURE( size == 1 );
			DAW_SAFE_POINTER_ENSURE( base );
		}

		[[nodiscard]] constexpr pointer get( ) const noexcept {
			return m_pointer;
		}

		constexpr pointer destroy( ) noexcept {
			m_destroyed = true;
			pointer result = get( );
			DAW_SAFE_POINTER_ENSURE( result );
			return result;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const noexcept {
			return not m_destroyed and static_cast<bool>( m_pointer );
		}

		template<typename Integer,
		         std::enable_if<( std::is_integral_v<Integer> and
		                          sizeof( Integer ) == sizeof( std::uintptr_t ) ),
		                        std::nullptr_t> = nullptr>
		[[nodiscard]] inline explicit operator Integer( ) const {
			DAW_SAFE_POINTER_ENSURE( *this );
			return reinterpret_cast<Integer>( get( ) );
		}

		template<typename U, std::enable_if_t<std::is_constructible_v<U *, pointer>,
		                                      std::nullptr_t> = nullptr>
		inline explicit operator safe_pointer<U>( ) const {
			return safe_pointer<U>( static_cast<U *>( m_pointer ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator==( safe_pointer<U> const &rhs ) const noexcept {
			return std::equal_to<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator!=( safe_pointer<U> const &rhs ) const noexcept {
			return std::not_equal_to<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator<( safe_pointer<U> const &rhs ) const noexcept {
			return std::less<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator<=( safe_pointer<U> const &rhs ) const noexcept {
			return std::less_equal<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator>( safe_pointer<U> const &rhs ) const noexcept {
			return std::greater<>{ }( get( ), rhs.get( ) );
		}

		template<typename U>
		[[nodiscard]] constexpr bool
		operator>=( safe_pointer<U> const &rhs ) const noexcept {
			return std::greater_equal<>{ }( get( ), rhs.get( ) );
		}
	};

	template<typename T, typename D>
	constexpr bool operator==( std::nullptr_t,
	                           safe_pointer<T> const &rhs ) noexcept {
		return not rhs.get( );
	}

	template<typename T, typename D>
	constexpr bool operator==( safe_pointer<T> const &lhs,
	                           std::nullptr_t ) noexcept {
		return not lhs.get( );
	}

	template<typename T>
	constexpr bool operator!=( std::nullptr_t,
	                           safe_pointer<T> const &rhs ) noexcept {
		return rhs.get( );
	}

	template<typename T>
	constexpr bool operator!=( safe_pointer<T> const &lhs,
	                           std::nullptr_t ) noexcept {
		return lhs.get( );
	}

} // namespace daw::memory

namespace std {
	template<typename T>
	struct iterator_traits<daw::memory::safe_pointer<T>> {
		using iterator_concept = contiguous_iterator_tag;
		using iterator_category = random_access_iterator_tag;
		using value_type = typename daw::memory::safe_pointer<T>::value_type;
		using difference_type =
		  typename daw::memory::safe_pointer<T>::difference_type;
		using pointer = daw::memory::safe_pointer<T>;
		using reference = typename daw::memory::safe_pointer<T>::reference;
	};

	template<typename T>
	struct pointer_traits<daw::memory::safe_pointer<T>> {
		using pointer = daw::memory::safe_pointer<T>;
		using element_type = T;
		using difference_type = typename pointer::difference_type;

		template<typename Element,
		         std::enable_if_t<
		           std::is_same_v<element_type, std::remove_reference_t<Element>>,
		           std::nullptr_t> = nullptr>
		static constexpr pointer pointer_to( Element &&r ) noexcept {
			return pointer( std::addressof( r ) );
		}

		template<typename U>
		requires( std::is_constructible_v<U *, T *> ) using rebind =
		  daw::memory::safe_pointer<U>;

		static constexpr element_type *to_address( pointer p ) noexcept {
			return p.get( );
		}
	};
} // namespace std