// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_string_view.h"

namespace daw {
	namespace impl {
		// Inspired by
		// https://github.com/jfalcou/trickeroo/blob/main/include/describe.hpp
		template<typename T>
		constexpr auto describe_impl( ) noexcept {
#ifdef __clang__
			constexpr auto prefix =
			  daw::string_view( "auto describe_impl() [T = " ).size( );
			constexpr auto suffix = daw::string_view( "]" ).size( );
			constexpr auto name_size =
			  daw::string_view( __PRETTY_FUNCTION__ ).size( );
			constexpr auto name = []<std::size_t... Is>( ) {
				constexpr auto tmp = daw::string_view( __PRETTY_FUNCTION__ )
				                       .remove_prefix( prefix )
				                       .remove_suffix( suffix );
				return std::array<char const, name_size - ( suffix + prefix ) + 1>{
				  tmp[Is]... };
			}
			( std::make_index_sequence<name_size>{ } );
#elif defined( __GNUC__ )
			auto name = daw::string_view( __PRETTY_FUNCTION__ );
			constexpr auto prefix =
			  daw::string_view( "constexpr auto describe_impl() [with T = " ).size( );
			constexpr auto suffix = daw::string_view( "]" ).size( );
#elif defined( _MSC_VER )
			auto name = daw::string_view( __FUNCSIG__ );
			constexpr auto prefix =
			  daw::string_view( "auto __cdecl describe_impl<" ).size( );
			constexpr auto suffix = daw::string_view( ">(void)" ).size( );
#endif
			name.remove_prefix( prefix );
			name.remove_suffix( suffix );
			return name;
		}
	} // namespace impl
	template<typename T>
	inline constexpr daw::string_view describe_v = impl::describe_impl<T>( );

} // namespace daw
