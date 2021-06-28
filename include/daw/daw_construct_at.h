// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"

#include <memory>
#include <new>
#include <type_traits>

namespace daw {
#if not defined( __cpp_lib_constexpr_dynamic_alloc )

	template<typename T, typename... Args>
	inline T *construct_at( T *p, Args &&...args ) {
		if constexpr( std::is_aggregate_v<T> ) {
			return ::new(
			  const_cast<void *>( static_cast<const volatile void *>( p ) ) )
			  T{ std::forward<Args>( args )... };
		} else {
			return ::new(
			  const_cast<void *>( static_cast<const volatile void *>( p ) ) )
			  T( std::forward<Args>( args )... );
		}
	}
}
#define DAW_CONSTRUCT_AT( ... ) ::daw::construct_at( __VA_ARGS__ )

#else

#define DAW_CONSTRUCT_AT( ... ) ::std::construct_at( __VA_ARGS__ )

#endif

