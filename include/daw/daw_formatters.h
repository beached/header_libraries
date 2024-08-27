// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_remove_cvref.h"
#include "daw/daw_traits.h"
#include "daw_format_range.h"
#include "daw_format_tuple.h"

namespace daw {
	template<typename T>
	[[nodiscard]] constexpr decltype( auto ) fmt_any( T &&v ) {
		using type = daw::remove_cvref_t<T>;

		if constexpr( Range<T> ) {
			return daw::fmt_range( DAW_FWD( v ) );
		} else if constexpr( daw::is_tuple_like_v<type> ) {
			return daw::fmt_tuple( DAW_FWD( v ) );
		} else {
			return DAW_FWD( v );
		}
	}
} // namespace daw
