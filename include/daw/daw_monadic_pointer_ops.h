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
	DAW_ATTRIB_FLATINLINE constexpr auto and_then( Pointer &&p, F &&f ) {
		static_assert(
		  std::is_invocable_v<F, Pointer>,
		  "The function passed to and_then is not callable with the current object "
		  "passed.  Does the pointer object need to be moved?" );
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
		return DAW_FWD( p );
	}

	template<typename Pointer, typename F>
	DAW_ATTRIB_FLATINLINE constexpr auto transform( Pointer &&p, F &&f ) {
		return daw::invoke( DAW_FWD( f ), DAW_FWD( p ) );
	}
} // namespace daw::monadic_ptr
