// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_remove_cvref.h"
#include "daw/daw_string_view_version.h"
#include "daw/impl/daw_make_trait.h"

#include <type_traits>

namespace daw {
#if DAW_STRINGVIEW_VERSION == 2
	inline
#endif
	  namespace sv2 {
		/// @brief The class template basic_string_view describes an object that can
		/// refer to a constant contiguous sequence of char-like objects with the
		/// first element of the sequence at position zero.
		template<typename CharT>
		struct basic_string_view;

		DAW_MAKE_REQ_TRAIT_TYPE( is_daw_string_view,
		                         daw::remove_cvref_t<T>::i_am_a_daw_string_view2 );

		using string_view = basic_string_view<char>;
		using wstring_view = basic_string_view<wchar_t>;

#if defined( __cpp_char8_t )
		using u8string_view = basic_string_view<char8_t>;
#endif
		using u16string_view = basic_string_view<char16_t>;
		using u32string_view = basic_string_view<char32_t>;
	} // namespace sv2
} // namespace daw
