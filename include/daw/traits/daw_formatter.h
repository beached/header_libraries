// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <format>

namespace daw {
	template<typename T, typename CharT = char>
	concept has_std_formatter_specialization_v = std::is_default_constructible_v<
	  std::formatter<daw::remove_cvref_t<T>, CharT>>;
}
