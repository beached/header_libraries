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
	struct formatter<daw::basic_string_view<CharT, Bounds>, CharT> {
		std::formatter<std::basic_string_view<CharT>> fmter{ };

		explicit formatter( ) = default;

		template<class ParseContext>
		constexpr ParseContext::iterator parse( ParseContext &ctx ) {
			return fmter.parse( ctx );
		}

		template<typename Ctx>
		Ctx::iterator format( daw::basic_string_view<CharT, Bounds> sv,
		                      Ctx &ctx ) const {
			return fmter( std::basic_string_view<CharT>( sv.data( ), sv.size( ) ),
			              ctx );
		}
	};
} // namespace std
