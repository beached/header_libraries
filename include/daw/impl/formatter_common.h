// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_string_view.h"

#include <cassert>
#include <format>
#include <string_view>

namespace daw::formatter_impl {
#if defined( WIN32 ) and not defined( DAW_RANGE_FORMAT_AS_CHAR )
	using DefaultCharT = wchar_t;
#else
	using DefaultCharT = char;
#endif
	template<typename>
	inline constexpr daw::basic_string_view DefaultSeparator = ",";
	template<typename>
	inline constexpr daw::basic_string_view DefaultLeft = "[";
	template<typename>
	inline constexpr daw::basic_string_view DefaultRight = "]";

	template<>
	inline constexpr daw::basic_string_view DefaultSeparator<wchar_t> = L",";
	template<>
	inline constexpr daw::basic_string_view DefaultLeft<wchar_t> = L"[";
	template<>
	inline constexpr daw::basic_string_view DefaultRight<wchar_t> = L"]";
} // namespace daw::formatter_impl
