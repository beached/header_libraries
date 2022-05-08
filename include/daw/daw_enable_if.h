// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_cpp_feature_check.h"

#include <cstddef>
#include <type_traits>

namespace daw {
	template<bool B, typename T = std::nullptr_t>
	using enable_if = std::enable_if<B, T>;

	template<bool B, typename T = std::nullptr_t>
	using enable_if_t = std::enable_if_t<B, T>;

	template<bool... B>
	using enable_when_t = enable_if_t<( B && ... )>;

	template<typename... Traits>
	using enable_when_all_t = enable_if_t<std::conjunction_v<Traits...>>;
} // namespace daw
