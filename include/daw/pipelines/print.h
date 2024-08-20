// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_print.h"
#include "daw/daw_typeof.h"
#include "range.h"

#include <type_traits>

namespace daw::pipelines {
	namespace pimple {
		struct Print_t {

			DAW_ATTRIB_NOINLINE
			[[nodiscard]] inline decltype( auto ) operator( )( auto &&r ) const {
				using R = DAW_TYPEOF( r );
				if constexpr( is_tuple_like_v<R> ) {
					daw::println( "{}", daw::fmt_tuple( r ) );
				} else if constexpr( Range<R> ) {
					daw::println( "{}", daw::fmt_range( r ) );
				} else {
					daw::println( "{}", r );
				}
				return DAW_FWD( r );
			}
		};
	} // namespace pimple

	inline constexpr auto Print = pimple::Print_t{ };
} // namespace daw::pipelines
