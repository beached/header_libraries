// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"

#include <type_traits>

namespace daw::traits {
	//////////////////////////////////////////////////////////////////////////
	/// Summary:	Is type T on of the other types
	///
	template<typename T, typename... Types>
	inline constexpr bool is_one_of_v = ( std::is_same_v<T, Types> or ... );

	template<typename T, typename... Types>
	using is_one_of_t = std::bool_constant<is_one_of_v<T, Types...>>;
} // namespace daw::traits
