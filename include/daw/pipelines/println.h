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
	namespace impl {
		struct PrintLn_t {
			DAW_ATTRIB_NOINLINE
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP inline decltype( auto )
			operator( )( auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				if constexpr( not Range<DAW_TYPEOF( r )> ) {
					daw::println( "{}", r );
				} else {
					daw::println( "{}", daw::fmt_range( r ) );
				}
				return DAW_FWD( r );
			}
		};
	} // namespace impl
	inline constexpr auto PrintLn = impl::PrintLn_t{ };
} // namespace daw::pipelines
