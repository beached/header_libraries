// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw {
	template<typename...>
	struct always_false {
		explicit always_false( int ) = delete;

		static constexpr bool value = false;
	};

	template<typename...Ts>
	inline constexpr bool always_false_v = always_false<Ts...>::value;
}


