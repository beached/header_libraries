// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/stdinc/remove_cvref.h>

#include <type_traits>

namespace daw {
	template<typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	template<typename T>
	using remove_cvrvref_t =
	  std::conditional_t<std::is_rvalue_reference_v<T>, remove_cvref_t<T>, T>;
} // namespace daw
