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
#include "daw/pipelines/range.h"

namespace daw::pipelines::pimpl {
	struct Copy_t {
		explicit Copy_t( ) = default;

		template<typename T>
		DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( T &&value ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return DAW_FWD( value );
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	constexpr auto Copy( ) {
		return pimpl::Copy_t{ };
	}
} // namespace daw::pipelines
