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

#include <cstdio>
#include <format>
#include <utility>

namespace daw {
	template<typename... Args>
	void print( FILE *fout, std::format_string<Args...> fmt, Args &&...args ) {
		auto const r = std::format( std::move( fmt ), DAW_FWD( args )... );
		(void)std::fwrite( std::data( r ), 1, r.size( ), fout );
	}

	template<typename... Args>
	void print( std::format_string<Args...> fmt, Args &&...args ) {
		daw::print( stdout, std::move( fmt ), DAW_FWD( args )... );
	}

	template<typename... Args>
	void println( FILE *f, std::format_string<Args...> fmt, Args &&...args ) {
		daw::print(
		  f, "{}\n", std::format( std::move( fmt ), DAW_FWD( args )... ) );
	}

	template<typename... Args>
	void println( std::format_string<Args...> fmt, Args &&...args ) {
		daw::print(
		  stdout, "{}\n", std::format( std::move( fmt ), DAW_FWD( args )... ) );
	}

	void println( ) {
		std::putchar( '\n' );
	}
} // namespace daw
