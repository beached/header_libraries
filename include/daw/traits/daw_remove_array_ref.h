// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>

namespace daw {
	template<typename T>
	struct remove_array_ref;

	template<typename T, std::size_t N>
	struct remove_array_ref<T ( & )[N]> {
		using type = T[N];
	};
	template<typename T>
	using remove_array_ref_t = typename remove_array_ref<T>::type;
} // namespace daw
