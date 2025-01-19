// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/traits/daw_remove_array_ref.h"

#include <functional>
#include <type_traits>

namespace daw {
	template<typename T, typename Compare = std::equal_to<>,
	         typename U = std::enable_if_t<std::is_array_v<remove_array_ref_t<T>>,
	                                       remove_array_ref_t<T>>>
	constexpr bool array_cmp( T &&lhs, T &&rhs,
	                          Compare const &cmp = Compare{ } ) {
		for( size_t n = 0; n < std::extent_v<U>; ++n ) {
			if constexpr( std::rank_v<U> == 1 ) {
				if( not cmp( lhs[n], rhs[n] ) ) {
					return false;
				}
			} else {
				if( not array_cmp( lhs[n], rhs[n], cmp ) ) {
					return false;
				}
			}
		}
		return true;
	}
} // namespace daw
