// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_attributes.h"
#include "daw_traits.h"

#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <utility>

namespace daw {
	template<typename CharT = char>
	DAW_ATTRIB_NOINLINE std::optional<std::basic_string<CharT>>
	read_file( std::string const &path ) noexcept {
		auto const fsize = std::filesystem::file_size( path );
		if( fsize == 0 ) {
			return std::basic_string<CharT>{ };
		}
		auto result = std::basic_string<CharT>( fsize, CharT{ } );
		auto *f = fopen( path.c_str( ), "rb" );
		if( not f ) {
			return std::nullopt;
		}
		auto num_read = fread( result.data( ), sizeof( CharT ), result.size( ), f );
		if( num_read != ( result.size( ) / sizeof( CharT ) ) ) {
			return std::nullopt;
		}
		return result;
	}

	struct terminate_on_read_file_error_t {};
	inline constexpr auto terminate_on_read_file_error =
	  terminate_on_read_file_error_t{ };

	inline std::string read_file( std::string const &path,
	                              terminate_on_read_file_error_t ) noexcept {
		auto result = read_file( path );
		if( not result ) {
			std::cerr << "Error: could not open file '" << path << "'\n";
			std::terminate( );
		}
		return *result;
	}

#if defined( _MSC_VER )
	DAW_ATTRIB_NOINLINE inline std::optional<std::wstring>
	read_wfile( std::wstring path ) noexcept {
		using CharT = wchar_t;
		auto const fsize = std::filesystem::file_size( path );
		if( fsize == 0 ) {
			return std::basic_string<CharT>{ };
		}
		auto result = std::basic_string<CharT>( fsize, CharT{ } );
		auto *f = _wfopen( path.c_str( ), "rb" );
		if( not f ) {
			return std::nullopt;
		}
		auto num_read = fread( result.data( ), sizeof( CharT ), result.size( ), f );
		if( num_read != ( result.size( ) / sizeof( CharT ) ) ) {
			return std::nullopt;
		}
		return result;
	}

	DAW_ATTRIB_NOINLINE inline std::wstring
	read_wfile( std::wstring const &path,
	            terminate_on_read_file_error_t ) noexcept {
		auto result = read_wfile( path );
		if( not result ) {
			std::cerr << "Error: could not open file '" << path << "'\n";
			std::terminate( );
		}
		return *result;
	}
#endif
} // namespace daw
