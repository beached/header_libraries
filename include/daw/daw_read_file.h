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
#include <iostream>
#include <iterator>
#include <optional>
#include <string>

namespace daw {
	template<typename String>
	std::optional<std::string> read_file( String &&path ) {
		using CharT = char;
		using result_t = std::optional<std::string>;
		CharT const *ptr = [&] {
			if constexpr( std::is_pointer_v<DAW_TYPEOF( path )> ) {
				return path;
			} else {
				return std::data( path );
			}
		}( );
		auto in_file = std::basic_ifstream<CharT>( ptr );
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
	std::string read_file( String &&path, terminate_on_read_file_error_t ) {
		auto result = read_file( DAW_FWD( path ) );
		if( not result ) {
			std::cerr << "Error: could not open file '" << path << "'\n";
			std::terminate( );
		}
		return *result;
	}

#if defined( _MSC_VER )
	template<typename String>
	std::optional<std::wstring> read_wfile( String &&path ) {
		using CharT = wchar_t;
		using result_t = std::optional<std::wstring>;
		CharT const *ptr = [&] {
			if constexpr( std::is_pointer_v<DAW_TYPEOF( path )> ) {
				return path;
			} else {
				return std::data( path );
			}
		}( );
		auto in_file = std::basic_ifstream<CharT>( ptr );
		if( not in_file ) {
			return result_t{ };
		}
		auto first = std::istreambuf_iterator<CharT>( in_file );
		auto last = std::istreambuf_iterator<CharT>( );
		return result_t( std::in_place, first, last );
	}

	template<typename String>
	std::wstring read_wfile( String &&path, terminate_on_read_file_error_t ) {
		auto result = read_wfile( DAW_FWD( path ) );
		if( not result ) {
			std::cerr << "Error: could not open file '" << path << "'\n";
			std::terminate( );
		}
		return *result;
	}
#endif
} // namespace daw
