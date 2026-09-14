// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_enable_requires.h"
#include "daw/daw_move.h"
#include "daw/impl/daw_make_trait.h"

#include <type_traits>
#include <utility>

namespace daw {
	namespace as_impl {
		DAW_MAKE_REQ_TRAIT2( explicitly_convertible_to_v,
		                     static_cast<T>( std::declval<U>( ) ) );
	}

	template<typename To, typename From DAW_ENABLEIF(
	                        as_impl::explicitly_convertible_to_v<To, From> )>
	DAW_REQUIRES( as_impl::explicitly_convertible_to_v<To, From> )
	[[nodiscard]] DAW_ATTRIB_INSTRINSIC DAW_ATTRIB_INLINE constexpr To as(
	  From &&from ) noexcept( noexcept( static_cast<To>( DAW_FWD( from ) ) ) ) {
		return static_cast<To>( DAW_FWD( from ) );
	}
} // namespace daw
