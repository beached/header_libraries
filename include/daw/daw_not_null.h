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
	template<class Pointer>
	struct not_null {
		using pointer = Pointer;
		using reference = typename std::iterator_traits<pointer>::reference;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using value_type = typename std::iterator_traits<pointer>::value_type;
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

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer get( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference
		operator*( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return get( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr
		operator pointer( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return get( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer
		operator->( ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
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

		DAW_ATTRIB_INLINE constexpr not_null &operator++( ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			++m_ptr;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr not_null &operator--( ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			--m_ptr;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr not_null operator++( int ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			auto result = *this;
			++m_ptr;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr not_null operator--( int ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			auto result = *this;
			--m_ptr;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr not_null &
		operator+=( std::ptrdiff_t n ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			m_ptr += n;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr not_null &
		operator-=( std::ptrdiff_t n ) noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			m_ptr -= n;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr reference
		operator[]( std::ptrdiff_t idx ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr[idx];
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr std::ptrdiff_t
		operator-( not_null const &rhs ) const noexcept {
			DAW_ASSUME( m_ptr != nullptr );
			return m_ptr -= rhs.m_ptr;
		}
	};

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr not_null<Pointer>
	operator-( not_null<Pointer> lhs, std::ptrdiff_t n ) noexcept {
		DAW_ASSUME( lhs.m_ptr != nullptr );
		lhs -= n;
		return lhs;
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr not_null<Pointer>
	operator+( not_null<Pointer> lhs, std::ptrdiff_t n ) noexcept {
		DAW_ASSUME( lhs.m_ptr != nullptr );
		lhs += n;
		return lhs;
	}

	template<typename Pointer>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr not_null<Pointer>
	operator+( std::ptrdiff_t n, not_null<Pointer> rhs ) noexcept {
		DAW_ASSUME( rhs.m_ptr != nullptr );
		rhs += n;
		return rhs;
	}
} // namespace daw

namespace std {
	template<typename Pointer>
	struct hash<daw::not_null<Pointer>> {
		[[nodiscard]] std::size_t
		operator( )( daw::not_null<Pointer> value ) const {
			return hash<decltype( std::declval<Pointer>( ) )>{ }( value.get( ) );
		}
	};
} // namespace std
