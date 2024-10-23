// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_formatters.h"
#include "daw_move.h"
#include "traits/daw_formatter.h"

#include <cstdio>
#include <format>
#include <utility>

namespace daw {
	template<typename... Args>
	DAW_ATTRIB_NOINLINE void print( FILE *fout, std::format_string<Args...> fmt,
	                                Args &&...args ) {
		auto const r = std::format( std::move( fmt ), DAW_FWD( args )... );
		(void)std::fwrite( std::data( r ), 1, r.size( ), fout );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void vprint( FILE *fout, std::string_view fmt,
	                                 Args &&...args ) {
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
	DAW_ATTRIB_NOINLINE void println( FILE *f, std::format_string<Args...> fmt,
	                                  Args &&...args ) {
		daw::print( f, "{}\n",
		            std::format( std::move( fmt ), DAW_FWD( args )... ) );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void vprintln( FILE *f, std::string fmt,
	                                   Args &&...args ) {
		fmt += '\n';
		daw::print( f, fmt, DAW_FWD( args )... );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void println( std::format_string<Args...> fmt,
	                                  Args &&...args ) {
		daw::print( stdout, "{}\n",
		            std::format( std::move( fmt ), DAW_FWD( args )... ) );
	}

	template<typename... Args>
	DAW_ATTRIB_NOINLINE void vprintln( std::string fmt, Args &&...args ) {
		fmt += '\n';
		daw::print( stdout, fmt, DAW_FWD( args )... );
	}

	DAW_ATTRIB_INLINE void println( ) {
		std::putchar( '\n' );
	}

	template<typename... Args>
	void dump( Args &&...args ) {
		static constexpr auto fmt = [] {
			if constexpr( sizeof...( Args ) == 0 ) {
				return std::array<char, 0>{ };
			} else {
				auto result = std::array<char, ( 3 * sizeof...( Args ) )>{ };
				for( std::size_t n = 0; n < result.size( ); n += 3 ) {
					result[n] = '{';
					result[n + 1] = '}';
					result[n + 2] = ' ';
				}
				return result;
			}
		}( );
		daw::println( std::format_string<Args...>(
		                std::string_view( fmt.data( ), fmt.size( ) ) ),
		              DAW_FWD( args )... );
	}
} // namespace daw
