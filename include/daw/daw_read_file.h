// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_as.h"
#include "daw/daw_attributes.h"
#include "daw/daw_cfile_ptr.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_int_cmp.h"
#include "daw/daw_string_view.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"

#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace daw {
	template<typename CharT = char>
	DAW_ATTRIB_NOINLINE std::optional<std::basic_string<CharT>>
	read_file( daw::string_view path ) {
		auto ec = std::error_code{ };
		auto const fsize_tmp =
		  std::filesystem::file_size( std::string_view( path ), ec );
		if( ec ) {
			return std::nullopt;
		}
		auto const fsize = daw::narrow_cast<std::size_t>( fsize_tmp );
		if( fsize % sizeof( CharT ) != 0 ) {
			return std::nullopt;
		}
		auto element_count = fsize / sizeof( CharT ) > 0 ? fsize / sizeof( CharT )
		                                                 : std::size_t{ 4096 };

		if( daw::cmp_less( std::basic_string<CharT>{ }.max_size( ),
		                   element_count ) ) {
			// File is too big to fit into string(WIN32)
			return std::nullopt;
		}
		auto const f = daw::open_cfile( path, "rb" );
		if( not f ) {
			return std::nullopt;
		}
		auto result = std::basic_string<CharT>( );
		bool keep_going = true;
#if defined( DAW_HAS_CPP23_STR_RESIZE_OVERWRITE )
		while( keep_going ) {
			auto const old_size = result.size( );
			result.resize_and_overwrite(
			  old_size + element_count, [&]( CharT *p, std::size_t ) {
				  std::advance( p, as<std::ptrdiff_t>( old_size ) );
				  auto const num_read =
				    std::fread( p, sizeof( CharT ), element_count, f.get( ) );
				  keep_going = num_read == element_count;
				  return old_size + num_read;
			  } );
			if( ferror( f.get( ) ) ) {
				return std::nullopt;
			}
			if( keep_going ) {
				// Ensure we don't expand too much and read efficiently
				element_count = std::size_t{ 4096 };
			}
		}
#else
		while( keep_going ) {
			auto const old_size = result.size( );
			result.resize( old_size + element_count, CharT{ } );
			CharT *p = std::next( result.data( ), as<std::ptrdiff_t>( old_size ) );
			auto const num_read =
			  std::fread( p, sizeof( CharT ), element_count, f.get( ) );
			if( ferror( f.get( ) ) ) {
				return std::nullopt;
			}
			keep_going = num_read == element_count;
			if( keep_going ) {
				// Ensure we don't expand too much and read efficiently
				element_count = std::size_t{ 4096 };
			} else {
				result.resize( old_size + num_read );
			}
		}
#endif
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
		auto const fsize_tmp =
		  std::filesystem::file_size( std::wstring_view( path ), ec );
		if( ec ) {
			return std::nullopt;
		}
		auto const fsize = daw::narrow_cast<std::size_t>( fsize_tmp );
		if( fsize % sizeof( CharT ) != 0 ) {
			return std::nullopt;
		}
		auto element_count = fsize / sizeof( CharT ) > 0 ? fsize / sizeof( CharT )
		                                                 : std::size_t{ 4096 };

		if( daw::cmp_less( std::basic_string<CharT>{ }.max_size( ),
		                   element_count ) ) {
			// File is too big to fit into string(WIN32)
			return std::nullopt;
		}
		auto const f = daw::open_wcfile( path, L"rb" );
		if( not f ) {
			return std::nullopt;
		}
		auto result = std::basic_string<CharT>( );
		bool keep_going = true;
#if defined( DAW_HAS_CPP23_STR_RESIZE_OVERWRITE )
		while( keep_going ) {
			auto const old_size = result.size( );
			result.resize_and_overwrite(
			  old_size + element_count, [&]( CharT *p, std::size_t ) {
				  std::advance( p, as<std::ptrdiff_t>( old_size ) );
				  auto const num_read =
				    std::fread( p, sizeof( CharT ), element_count, f.get( ) );
				  keep_going = num_read == element_count;
				  return old_size + num_read;
			  } );
			if( ferror( f.get( ) ) ) {
				return std::nullopt;
			}
			if( keep_going ) {
				// Ensure we don't expand too much and read efficiently
				element_count = std::size_t{ 4096 };
			}
		}
#else
		while( keep_going ) {
			auto const old_size = result.size( );
			result.resize( old_size + element_count, CharT{ } );
			CharT *p = std::next( result.data( ), as<std::ptrdiff_t>( old_size ) );
			auto const num_read =
			  std::fread( p, sizeof( CharT ), element_count, f.get( ) );
			if( ferror( f.get( ) ) ) {
				return std::nullopt;
			}
			keep_going = num_read == element_count;
			if( keep_going ) {
				// Ensure we don't expand too much and read efficiently
				element_count = std::size_t{ 4096 };
			} else {
				result.resize( old_size + num_read );
			}
		}
#endif
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
