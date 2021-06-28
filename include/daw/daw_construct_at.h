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

	template<typename T, typename Storage, typename... Args>
	inline T *construct_at( Storage *p, Args &&...args ) {
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
#else

	template<typename T, typename Storage, typename... Args>
	inline constexpr T *construct_at( Storage *p, Args &&...args ) {
		return std::construct_at( reinterpret_cast<T*>( p ), DAW_FWD( args )... );
	}
#endif
}

