// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_attributes.h"
#include "daw/daw_string_view.h"
#include "daw/daw_traits.h"

#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace daw {
	template<typename CharT = char>
	DAW_ATTRIB_NOINLINE std::optional<std::basic_string<CharT>>
	read_file( daw::string_view path ) {
		auto ec = std::error_code{ };
		auto const fsize =
		  std::filesystem::file_size( std::string_view( path ), ec );
		if( ec ) {
			return std::nullopt;
		}
		if( fsize % sizeof( CharT ) != 0 ) {
			return std::nullopt;
		}
		auto const element_count = fsize / sizeof( CharT );
		if( static_cast<std::uintmax_t>( std::basic_string<CharT>{ }.max_size( ) ) <
		    element_count ) {
			// File is too big to fit into string(WIN32)
			return std::nullopt;
		}
		auto result = std::basic_string<CharT>(
		  static_cast<std::size_t>( element_count ), CharT{ } );
#if defined( _MSC_VER )
		FILE *f = nullptr;
		auto err = fopen_s( &f, path.get_c_str( ).c_str( ), "rb" );
		if( err or not f ) {
			return std::nullopt;
		}
#else
		auto *f = fopen( path.get_c_str( ).c_str( ), "rb" );
		if( not f ) {
			return std::nullopt;
		}
#endif
		auto const num_read =
		  fread( result.data( ), sizeof( CharT ), result.size( ), f );
		auto const close_result = fclose( f );
		if( num_read != result.size( ) or close_result != 0 ) {
			return std::nullopt;
		}
		return result;
	}

	struct terminate_on_read_file_error_t {};
	inline constexpr auto terminate_on_read_file_error =
	  terminate_on_read_file_error_t{ };

	inline std::string read_file( daw::string_view path,
	                              terminate_on_read_file_error_t ) {
		auto result = read_file( path );
		if( not result ) {
			std::cerr << "Error: could not open file '" << path << "'\n";
			std::terminate( );
		}
		return std::move( *result );
	}

#if defined( _MSC_VER )
	DAW_ATTRIB_NOINLINE inline std::optional<std::wstring>
	read_wfile( daw::wstring_view path ) {
		using CharT = wchar_t;
		auto ec = std::error_code{ };
		auto const fsize =
		  std::filesystem::file_size( std::wstring_view( path ), ec );
		if( ec ) {
			return std::nullopt;
		}
		if( fsize % sizeof( CharT ) != 0 ) {
			return std::nullopt;
		}
		auto const element_count = fsize / sizeof( CharT );
		if( static_cast<std::uintmax_t>( std::basic_string<CharT>{ }.max_size( ) ) <
		    element_count ) {
			// File is too big to fit into string(WIN32)
			return std::nullopt;
		}
		auto result = std::basic_string<CharT>(
		  static_cast<std::size_t>( element_count ), CharT{ } );
		FILE *f = nullptr;
		auto err = _wfopen_s( &f, path.get_c_str( ).c_str( ), L"rb" );
		if( err or not f ) {
			return std::nullopt;
		}
		auto const num_read =
		  fread( result.data( ), sizeof( CharT ), result.size( ), f );
		auto const close_result = fclose( f );
		if( num_read != result.size( ) or close_result != 0 ) {
			return std::nullopt;
		}
		return result;
	}

	DAW_ATTRIB_NOINLINE inline std::wstring
	read_wfile( daw::wstring_view path, terminate_on_read_file_error_t ) {
		auto result = read_wfile( path );
		if( not result ) {
			std::wcerr << L"Error: could not open file '" << path << L'\n';
			std::terminate( );
		}
		return *result;
	}
#endif
} // namespace daw
