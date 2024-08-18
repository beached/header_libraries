// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_exchange.h"

#include <cstddef>
#include <daw/stdinc/data_access.h>
#include <daw/stdinc/move_fwd_exch.h>
#include <daw/stdinc/range_access.h>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace daw {
	template<typename T, typename Allocator>
	struct container_deleter : Allocator {
		using alloc_traits = std::allocator_traits<Allocator>;
		using pointer = typename alloc_traits::pointer;
		std::size_t capacity = 0;

		explicit container_deleter( ) = default;
		explicit constexpr container_deleter(
		  Allocator alloc,
		  std::size_t
		    sz ) noexcept( std::is_nothrow_move_constructible_v<Allocator> )
		  : Allocator( std::move( alloc ) )
		  , capacity( sz ) {}

		constexpr std::pair<Allocator, std::size_t> extract_allocator( ) {
			return std::pair<Allocator, std::size_t>{
			  std::move( *static_cast<Allocator *>( this ) ),
			  daw::exchange( capacity, 0U ) };
		}

		constexpr void operator( )( pointer p ) {
			auto count = capacity;
			if constexpr( not std::is_trivially_destructible_v<
			                typename std::pointer_traits<pointer>::element_type> ) {
				while( count-- > 0 ) {
					alloc_traits::destroy( *this, std::addressof( p[count] ) );
				}
			}
			alloc_traits::deallocate( *this, p, capacity );
			capacity = 0;
		}
	};

	template<typename T, typename Allocator>
	class container_data {
		using deleter_type = container_deleter<T, Allocator>;
		using data_type = std::unique_ptr<T, deleter_type>;
		using pointer = typename deleter_type::pointer;
		using size_type = std::size_t;

		data_type m_data = data_type( );

		size_type m_size = 0;

	public:
		explicit container_data( ) = default;

		explicit constexpr container_data( pointer buff,
		                                   Allocator alloc,
		                                   size_type sz,
		                                   size_type cap )
		  : m_data( buff, deleter_type( std::move( alloc ), cap ) )
		  , m_size( sz ) {}

		[[nodiscard]] constexpr pointer data( ) const noexcept {
			return m_data.get( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_size;
		}

		[[nodiscard]] constexpr size_type capacity( ) const noexcept {
			return m_data.get_deleter( ).capacity;
		}

		constexpr void reset( ) {
			m_data.reset( );
			m_size = 0;
		}
	};
} // namespace daw
