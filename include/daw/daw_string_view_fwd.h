// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace daw {
	inline constexpr ptrdiff_t const dynamic_string_size = -1;

	struct StringViewBoundsPointer : std::bool_constant<true> {};
	struct StringViewBoundsSize : std::bool_constant<false> {};

	template<typename T>
	using is_string_view_bounds_type =
	  std::disjunction<std::is_same<T, StringViewBoundsPointer>,
	                   std::is_same<T, StringViewBoundsSize>>;

#if defined( _MSC_VER ) and not defined( __clang__ )
	// MSVC has issues with the second item being a pointer
	using default_string_view_bounds_type = StringViewBoundsSize;
#else
	using default_string_view_bounds_type = StringViewBoundsPointer;
#endif

	template<typename T>
	inline constexpr bool is_string_view_bounds_type_v =
	  is_string_view_bounds_type<T>::value;

	template<typename CharT,
	         typename BoundsType = default_string_view_bounds_type,
	         ptrdiff_t Extent = dynamic_string_size>
	struct basic_string_view;

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;
} // namespace daw
