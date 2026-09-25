// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"

#include <type_traits>
#include <utility>

namespace daw {
#if defined( DAW_HAS_CPP20_CONCEPTS )
	template<typename From, typename To>
	inline constexpr bool is_explicitly_convertible_v = requires {
		static_cast<To>( std::declval<From>( ) );
	};
#else
	template<typename, typename, typename = void>
	inline constexpr bool is_explicitly_convertible_v = false;

	template<typename From, typename To>
	inline constexpr bool is_explicitly_convertible_v<
	  From, To,
	  std::void_t<decltype( static_cast<To>( std::declval<From>( ) ) )>> = true;
#endif
} // namespace daw
