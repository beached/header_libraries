// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/stdinc/remove_cvref.h>

namespace daw {
	template<typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
}
