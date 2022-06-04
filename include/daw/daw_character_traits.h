// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#include <type_traits>

namespace daw::traits {
	template<typename T>
	inline constexpr bool is_character_type_v = false;

	template<>
	inline constexpr bool is_character_type_v<char> = true;

	template<>
	inline constexpr bool is_character_type_v<wchar_t> = true;
#if defined( __cpp_char8_t )
#if __cpp_char8_t >= 201907L
	template<>
	inline constexpr bool is_character_type_v<char8_t> = true;
#endif
#endif
	template<>
	inline constexpr bool is_character_type_v<char16_t> = true;

	template<>
	inline constexpr bool is_character_type_v<char32_t> = true;

	template<typename T>
	using is_character_type_t = std::bool_constant<is_character_type_v<T>>;
} // namespace daw::traits
