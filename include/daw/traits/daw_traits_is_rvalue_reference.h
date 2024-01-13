// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/stdinc/integer_sequence.h>

namespace daw::traits {
	template<typename>
	inline constexpr bool is_rvalue_reference_v = false;

	template<typename T>
	inline constexpr bool is_rvalue_reference_v<T &&> = true;

	template<typename T>
	using is_rvalue_reference = std::bool_constant<is_rvalue_reference_v<T>>;
} // namespace daw::traits
