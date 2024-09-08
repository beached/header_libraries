// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_string_view.h"

#include <format>
#include <string_view>

namespace std {
	template<typename CharT, daw::string_view_bounds_type Bounds>
	struct formatter<daw::sv2::basic_string_view<CharT, Bounds>, CharT>
	  : std::formatter<std::basic_string_view<CharT>, CharT> {
		using base = std::formatter<std::basic_string_view<CharT>, CharT>;

		template<typename Ctx>
		constexpr Ctx::iterator
		format( daw::sv2::basic_string_view<CharT, Bounds> sv, Ctx &ctx ) const {
			return base::format(
			  std::basic_string_view<CharT>( sv.data( ), sv.size( ) ), ctx );
		}
	};
} // namespace std
