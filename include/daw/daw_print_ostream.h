// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"
#include "daw_range_formatter.h"

#include <format>
#include <iostream>
#include <utility>

namespace daw {
	template<typename... Args>
	std::ostream &
	print( std::ostream &os, std::format_string<Args...> fmt, Args &&...args ) {
		auto const r = std::format( std::move( fmt ), DAW_FWD( args )... );
		(void)os.write( std::data( r ), r.size( ) );
		return os;
	}

	template<typename... Args>
	std::ostream &
	println( std::ostream &os, std::format_string<Args...> fmt, Args &&...args ) {
		return daw::print(
		  os, "{}\n", std::format( std::move( fmt ), DAW_FWD( args )... ) );
	}
} // namespace daw
