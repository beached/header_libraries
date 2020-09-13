// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once

#include <fstream>
#include <optional>
#include <string>

namespace daw {
	template<typename CharT>
	std::optional<std::basic_string<char>> read_file( CharT const *str ) {
		auto in_file = std::basic_ifstream<char>( str );
		if( not in_file ) {
			return { };
		}

		return std::basic_string<char>( std::istreambuf_iterator<char>( in_file ),
		                                { } );
	}

	template<typename CharT>
	std::optional<std::basic_string<char>>
	read_file( std::basic_string<CharT> str ) {
		return read_file( str.c_str( ) );
	}

	template<typename CharT>
	std::optional<std::basic_string<wchar_t>> read_wfile( CharT const *str ) {
		auto in_file = std::basic_ifstream<wchar_t>( str );
		if( not in_file ) {
			return { };
		}

		return std::basic_string<wchar_t>(
		  std::istreambuf_iterator<wchar_t>( in_file ), { } );
	}

	template<typename CharT>
	std::optional<std::basic_string<wchar_t>>
	read_wfile( std::basic_string<CharT> str ) {
		return read_wfile( str.c_str( ) );
	}
} // namespace daw
