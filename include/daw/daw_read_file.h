// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"
#include "daw_traits.h"

#include <ciso646>
#include <fstream>
#include <optional>
#include <string>

namespace daw {
	template<typename String>
	std::optional<std::string> read_file( String &&path ) {
		using CharT = char;
		using result_t = std::optional<std::string>;
		auto in_file = std::basic_ifstream<CharT>( std::data( path ) );
		if( not in_file ) {
			return result_t{ };
		}
		auto first = std::istreambuf_iterator<CharT>( in_file );
		auto last = std::istreambuf_iterator<CharT>( );
		return result_t( std::in_place, first, last );
	}

	struct terminate_on_read_file_error_t {};
	inline constexpr auto terminate_on_read_file_error =
	  terminate_on_read_file_error_t{ };

	template<typename String>
	std::optional<std::string> read_file( String &&path,
	                                      terminate_on_read_file_error_t ) {
		auto result = read_file( DAW_FWD( path ) );
		if( not result ) {
			std::cerr << "Error: could not open file '" << path << "'\n";
			std::terminate( );
		}
		return *result;
	}

	template<typename String>
	std::optional<std::wstring> read_wfile( String &&path ) {
		using CharT = wchar_t;
		using result_t = std::optional<std::wstring>;
		auto in_file = std::basic_ifstream<CharT>( std::data( path ) );
		if( not in_file ) {
			return result_t{ };
		}
		auto first = std::istreambuf_iterator<CharT>( in_file );
		auto last = std::istreambuf_iterator<CharT>( );
		return result_t( std::in_place, first, last );
	}

	template<typename String>
	std::optional<std::wstring> read_wfile( String &&path,
	                                        terminate_on_read_file_error_t ) {
		auto result = read_wfile( DAW_FWD( path ) );
		if( not result ) {
			std::cerr << "Error: could not open file '" << path << "'\n";
			std::terminate( );
		}
		return *result;
	}

} // namespace daw
