// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_swap.h"
#include "daw_traits.h"

#include <memory>
#include <type_traits>

namespace daw {
	/// Heap Value.  Access members via operator-> but copy/move constructors
	/// operators utilized the pointed to's members This is used on larger classes
	/// that are members of other classes but the space requirements is that of a
	/// pointer instead of the full size
	template<typename T>
	struct heap_value {
		using value_t = std::decay_t<T>;
		using reference = value_t &;
		using const_reference = value_t const &;
		using pointer = value_t *;
		using const_pointer = value_t const *;

		std::unique_ptr<value_t> m_value{ std::make_unique<value_t>( ) };

	public:
		heap_value( ) = default;
		heap_value( heap_value && ) noexcept = default;
		heap_value &operator=( heap_value && ) noexcept = default;
		~heap_value( ) = default;

		heap_value( heap_value const &other )
		  : m_value{ std::make_unique<value_t>( *other.m_value ) } {}

		heap_value &operator=( heap_value const &rhs ) {
			if( this != &rhs ) {
				*m_value = *rhs.m_value;
			}
			return *this;
		}

		// Make this less perfect so that we can still do copy/move construction
		template<typename Arg, typename = std::enable_if_t<
		                         daw::traits::not_self<Arg, value_t>( )>>
		heap_value( Arg &&arg )
		  : m_value{ std::make_unique<value_t>( DAW_FWD( arg ) ) } {}

		template<typename Arg, typename... Args>
		heap_value( Arg &&arg, Args &&...args )
		  : m_value{ std::make_unique<value_t>( DAW_FWD( arg ),
		                                        DAW_FWD( args )... ) } {}

		void swap( heap_value &rhs ) noexcept {
			daw::cswap( m_value, rhs.m_value );
		}

		reference get( ) {
			return *m_value;
		}

		const_reference get( ) const {
			return *m_value;
		}

		reference operator*( ) {
			return *m_value;
		}

		const_reference operator*( ) const {
			return *m_value;
		}

		pointer operator->( ) {
			return m_value.get( );
		}

		const_pointer operator->( ) const {
			return m_value.get( );
		}

		pointer ptr( ) {
			return m_value.get( );
		}

		const_pointer ptr( ) const {
			return m_value.get( );
		}

		template<typename... Args>
		auto operator[]( Args &&...args ) {
			return m_value->operator[]( DAW_FWD( args )... );
		}

		template<typename... Args>
		auto operator[]( Args &&...args ) const {
			return m_value->operator[]( DAW_FWD( args )... );
		}

		template<typename... Args>
		auto operator( )( Args &&...args ) {
			return m_value->operator[]( DAW_FWD( args )... );
		}

		template<typename... Args>
		auto operator( )( Args &&...args ) const {
			return m_value->operator[]( DAW_FWD( args )... );
		}
	};

	template<typename T>
	void swap( heap_value<T> &lhs, heap_value<T> &rhs ) noexcept {
		lhs.swap( rhs );
	}

	template<typename T, typename U>
	bool operator==( heap_value<T> const &lhs, heap_value<U> const &rhs ) {
		return *lhs == *rhs;
	}

	template<typename T, typename U>
	bool operator!=( heap_value<T> const &lhs, heap_value<U> const &rhs ) {
		return *lhs != *rhs;
	}

	template<typename T, typename U>
	bool operator>=( heap_value<T> const &lhs, heap_value<U> const &rhs ) {
		return *lhs >= *rhs;
	}

	template<typename T, typename U>
	bool operator<=( heap_value<T> const &lhs, heap_value<U> const &rhs ) {
		return *lhs <= *rhs;
	}

	template<typename T, typename U>
	bool operator>( heap_value<T> const &lhs, heap_value<U> const &rhs ) {
		return *lhs > *rhs;
	}

	template<typename T, typename U>
	bool operator<( heap_value<T> const &lhs, heap_value<U> const &rhs ) {
		return *lhs < *rhs;
	}
} // namespace daw
