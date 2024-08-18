// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#pragma once

#include "ciso646.h"
#include "daw_string_view_version.h"

namespace daw {
#if DAW_STRINGVIEW_VERSION == 2
	inline
#endif
	  namespace sv2 {
		/// @brief How is the end of range stored in string_view
		enum class string_view_bounds_type {
			/// @brief Store the end of range as a pointer. This is optimal for code
			/// where calls to methods like remove_prefix dominate
			pointer,
			/// @brief Store the end of range as a size_type.  This is optimal for
			/// where calls to size( ) or remove_suffix like calls dominate
			size
		};

#if defined( _MSC_VER ) or defined( DAW_SV_USE_PTRSIZE )
		// MSVC has issues with pointers to the trailing zero in a string literal at
		// compile time
		inline constexpr string_view_bounds_type default_string_view_bounds_type =
		  string_view_bounds_type::size;
#else
		inline constexpr string_view_bounds_type default_string_view_bounds_type =
		  string_view_bounds_type::pointer;
#endif

		/// @brief The class template basic_string_view describes an object that can
		/// refer to a constant contiguous sequence of char-like objects with the
		/// first element of the sequence at position zero.
		template<typename CharT,
		         string_view_bounds_type BoundsType =
		           default_string_view_bounds_type>
		struct basic_string_view;

		using string_view = basic_string_view<char>;
		using wstring_view = basic_string_view<wchar_t>;

#if defined( __cpp_char8_t )
		using u8string_view = basic_string_view<char8_t>;
#endif
		using u16string_view = basic_string_view<char16_t>;
		using u32string_view = basic_string_view<char32_t>;
	} // namespace sv2
} // namespace daw
