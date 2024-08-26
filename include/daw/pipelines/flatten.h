// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_deduced_type.h"

namespace daw::pipelines::pimpl {
	template<typename OutputType>
	struct Flatten_t {
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if constexpr( is_deduced_type_v<OutputType> ) {

			}
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<typename OutputType = daw::deduced_type>
	[[nodiscard]] constexpr auto Flatten( ) {}
} // namespace daw::pipelines
