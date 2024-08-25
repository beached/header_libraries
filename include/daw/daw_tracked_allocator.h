// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

// Experiment

#pragma once

#include "daw_attributes.h"
#include "daw_safe_pointer.h"
#include "traits/daw_traits_conditional.h"

#include <cassert>
#include <cstddef>
#include <memory>

namespace daw::memory {
	template<typename T, typename Allocator = std::allocator<T>>
	struct tracked_allocator {
		using value_type = T;
		static_assert(
		  std::is_same_v<value_type,
		                 typename std::allocator_traits<Allocator>::value_type> );
		using pointer = safe_pointer<value_type>;
		using const_pointer = safe_pointer<value_type const>;
		using void_pointer = safe_pointer<
		  conditional_t<std::is_const_v<value_type>, void const, void>>;
		using const_void_pointer = safe_pointer<void const>;
		using difference_type = typename safe_pointer<value_type>::difference_type;
		using size_type = typename safe_pointer<value_type>::size_type;
		using propagate_on_container_copy_assignment =
		  typename std::allocator_traits<
		    Allocator>::propagate_on_container_copy_assignment;
		using propagate_on_container_move_assignment =
		  typename std::allocator_traits<
		    Allocator>::propagate_on_container_move_assignment;
		using propagate_on_container_swap =
		  typename std::allocator_traits<Allocator>::propagate_on_container_swap;
		using is_always_equal =
		  typename std::allocator_traits<Allocator>::is_always_equal;
		using other = tracked_allocator;

		template<typename U>
		using rebind = tracked_allocator<
		  U,
		  typename std::allocator_traits<Allocator>::template rebind_alloc<U>>;

	private:
		DAW_NO_UNIQUE_ADDRESS Allocator alloc{ };

	public:
		tracked_allocator( ) = default;

		constexpr pointer allocate( size_type n ) {
			value_type *ptr = std::allocator_traits<Allocator>::allocate( alloc, n );
			return pointer( ptr, n );
		}

		constexpr void deallocate( pointer &ptr, size_type n ) noexcept {
			DAW_SAFE_POINTER_ENSURE( ptr );
			DAW_SAFE_POINTER_ENSURE( not ptr.get_base( ) or
			                         ( ptr.get( ) == ptr.get_base( ) ) );
			value_type *p = ptr.destroy( );
			std::allocator_traits<Allocator>::deallocate( alloc, p, n );
		}

		template<typename... Args>
		constexpr void construct( pointer &ptr, Args &&...args ) noexcept(
		  std::is_nothrow_constructible_v<T, Args...> ) {
			DAW_SAFE_POINTER_ENSURE( ptr );
			std::allocator_traits<Allocator>::construct(
			  alloc, ptr.get( ), DAW_FWD( args )... );
		}

		constexpr void
		destroy( pointer &ptr ) noexcept( std::is_nothrow_destructible_v<T> ) {
			DAW_SAFE_POINTER_ENSURE( ptr );
			std::allocator_traits<Allocator>::destroy( alloc, ptr.get( ) );
		}
	};
} // namespace daw::memory
