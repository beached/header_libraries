// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_tuple_forward.h"
#include "zip.h"

#include <algorithm>
#include <cstddef>
#include <tuple>

namespace daw::pipelines::pimpl {
	template<std::size_t... Indices>
	struct Swizzle_t {
		template<Range R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if constexpr( requires {
				              typename daw::remove_cvref_t<
				                R>::i_am_a_daw_zip_iterator_class;
			              } ) {
				return map_view{ std::begin( r ), std::end( r ), []( auto &&tp ) {
					                return daw::forward_nonrvalue_as_tuple(
					                  get<Indices>( DAW_FWD( tp ) )... );
				                } };
			} else if constexpr( is_tuple_like_v<range_value_t<R>> ) {
				using std::get;
				static_assert( std::max( { Indices... } ) <
				                 std::tuple_size_v<range_reference_t<R>>,
				               "Swizzle - Index out of range of tuple type" );
				return map_view{ std::begin( r ), std::end( r ), []( auto &&v ) {
					                return daw::forward_nonrvalue_as_tuple(
					                  get<Indices>( DAW_FWD( v ) )... );
				                } };
			} else {
				// Not a Range
				static_assert(
				  std::max( { Indices... } ) == 0,
				  "Swizzle requires only index 0 is valid for none tuple like types" );
				return zip_view{ ( (void)Indices, DAW_FWD( r ) )... };
			}
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<std::size_t... Indices>
	inline constexpr auto Swizzle = pimpl::Swizzle_t<Indices...>{ };
}