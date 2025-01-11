// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_apply.h"
#include "daw/daw_attributes.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"

#include <type_traits>

namespace daw::monadic_ptr {
	template<typename Pointer, typename F>
	DAW_ATTRIB_FLATINLINE constexpr auto and_then( Pointer &&p, F &&f )
	  -> daw::remove_cvref_t<std::invoke_result_t<F, Pointer>> {
		if( p ) {
			return daw::invoke( DAW_FWD( f ), DAW_FWD( p ) );
		}
		return daw::remove_cvref_t<std::invoke_result_t<F, Pointer>>{ };
	}

	template<typename Pointer, typename F>
	DAW_ATTRIB_FLATINLINE constexpr auto or_else( Pointer &&p, F &&f )
	  -> daw::remove_cvref_t<Pointer> {
		if( not p ) {
			return daw::invoke( DAW_FWD( f ) );
		}
		return p;
	}
} // namespace daw::monadic_ptr
