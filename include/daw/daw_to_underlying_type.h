// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <type_traits>

namespace daw {
	template<typename Enum,
	         std::enable_if_t<std::is_enum_v<Enum>, std::nullptr_t> = nullptr>
	constexpr auto to_underlying( Enum e ) {
		return static_cast<std::underlying_type_t<Enum>>( e );
	}
} // namespace daw
