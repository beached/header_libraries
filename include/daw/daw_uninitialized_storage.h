// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_move.h"
#include "daw_traits.h"

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	struct alignas( T ) uninitialized_storage : enable_copy_assignment<T>,
	                                            enable_copy_constructor<T>,
	                                            enable_move_assignment<T>,
	                                            enable_move_constructor<T> {
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		std::byte m_data[sizeof( value_type )];

	public:
		constexpr uninitialized_storage( ) noexcept = default;

		template<typename... Args>
		void construct( Args &&...args ) noexcept(
		  std::is_nothrow_constructible_v<T, Args...> ) {

			if constexpr( std::is_aggregate_v<T> ) {
				new( m_data ) T{ std::forward<Args>( args )... };
			} else {
				new( m_data ) T( std ::forward<Args>( args )... );
			}
		}

		pointer ptr( ) noexcept {
			return std::launder( reinterpret_cast<pointer>( m_data ) );
		}

		const_pointer ptr( ) const noexcept {
			return std::launder( reinterpret_cast<const_pointer>( m_data ) );
		}

		const_pointer cptr( ) const noexcept {
			return std::launder( reinterpret_cast<const_pointer>( m_data ) );
		}

		reference ref( ) noexcept {
			return *ptr( );
		}

		const_reference cref( ) const noexcept {
			return *cptr( );
		}

		value_type rref( ) noexcept {
			return value_type( std::move( *ptr( ) ) );
		}

		reference operator*( ) noexcept {
			return ref( );
		}

		const_reference operator*( ) const noexcept {
			return cref( );
		}

		pointer operator->( ) noexcept {
			return ptr( );
		}

		const_pointer operator->( ) const noexcept {
			return cptr( );
		}

		void destruct( ) noexcept( std::is_nothrow_destructible_v<T> ) {
			ptr( )->~T( );
		}

		constexpr std::byte *raw_data( ) const noexcept {
			return m_data;
		}
	};
} // namespace daw
