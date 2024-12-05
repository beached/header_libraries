// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_concepts.h"
#include "daw_move.h"

namespace daw {
	template<typename...Predicates>
	requires( (Fn<Predicates, bool()> or std::convertible_to<Predicates, bool>) and ...)
	constexpr bool any_if(  Predicates && ...predicates ) {
		constexpr auto const do_test = []( auto && p ) -> bool {
			if constexpr( Fn<DAW_TYPEOF( p ), bool()> ) {
				return p( );
			} else {
				return static_cast<bool>( p );
			}
		};
		return (do_test( DAW_FWD( predicates ) ) and ...);
	}
}
