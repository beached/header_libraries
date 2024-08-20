// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_format_range.h"
#include "daw_format_tuple.h"
#include "daw_move.h"
#include "traits/daw_formatter.h"

#include <cstdio>
#include <format>
#include <utility>

namespace daw {
	template<typename... Args>
	DAW_ATTRIB_NOINLINE void
	print( FILE *fout, std::format_string<Args...> fmt, Args &&...args ) {
		auto const r = std::format( std::move( fmt ), DAW_FWD( args )... );
		(void)std::fwrite( std::data( r ), 1, r.size( ), fout );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void
	vprint( FILE *fout, std::string_view fmt, Args &&...args ) {
		auto const r =
		  std::vformat( fmt, std::make_format_args( DAW_FWD( args )... ) );
		(void)std::fwrite( std::data( r ), 1, r.size( ), fout );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void print( std::format_string<Args...> fmt,
	                                Args &&...args ) {
		daw::print( stdout, std::move( fmt ), DAW_FWD( args )... );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void vprint( std::string_view fmt, Args &&...args ) {
		vprint( stdout, fmt, DAW_FWD( args )... );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void
	println( FILE *f, std::format_string<Args...> fmt, Args &&...args ) {
		daw::print(
		  f, "{}\n", std::format( std::move( fmt ), DAW_FWD( args )... ) );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void
	vprintln( FILE *f, std::string fmt, Args &&...args ) {
		fmt += '\n';
		daw::print( f, fmt, DAW_FWD( args )... );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void println( std::format_string<Args...> fmt,
	                                  Args &&...args ) {
		daw::print(
		  stdout, "{}\n", std::format( std::move( fmt ), DAW_FWD( args )... ) );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void vprintln( std::string fmt, Args &&...args ) {
		fmt += '\n';
		daw::print( stdout, fmt, DAW_FWD( args )... );
	}

	DAW_ATTRIB_INLINE void println( ) {
		std::putchar( '\n' );
	}
} // namespace daw
