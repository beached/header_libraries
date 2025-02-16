// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_ordering_to_integer.h"
#include "daw/daw_traits.h"
#include "daw/traits/daw_remove_array_ref.h"

#include <functional>
#include <type_traits>

namespace daw {
	namespace array_cmp_details {
#if defined( DAW_HAS_CPP20_3WAY )
		template<typename Order>
		inline constexpr Order equal_or_equivalent_v =
		  daw::equal_or_equivalent_v<Order>;
#else
		template<typename>
		inline constexpr bool equal_or_equivalent_v = true;
#endif
	} // namespace array_cmp_details

	template<typename T, typename Compare = std::equal_to<>,
	         typename U = std::enable_if_t<std::is_array_v<remove_array_ref_t<T>>,
	                                       remove_array_ref_t<T>>>
	DAW_ATTRIB_FLATTEN constexpr auto
	array_cmp( T &&lhs, T &&rhs, Compare const &cmp = Compare{ } ) {
		using compare_result_t =
		  std::invoke_result_t<Compare, std::remove_all_extents_t<U>,
		                       std::remove_all_extents_t<U>>;
		for( std::size_t n = 0; n < std::extent_v<U>; ++n ) {
			auto r = [&] {
				if constexpr( std::rank_v<U> > 1 ) {
					return array_cmp( lhs[n], rhs[n], cmp );
				} else {
					return cmp( lhs[n], rhs[n] );
				}
			}( );
			if( r != array_cmp_details::equal_or_equivalent_v<compare_result_t> ) {
				return r;
			}
		}
		return array_cmp_details::equal_or_equivalent_v<compare_result_t>;
	}
} // namespace daw
