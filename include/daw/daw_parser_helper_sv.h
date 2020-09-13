// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once

#include <cstddef>

#include "daw_parser_addons.h"
#include "daw_parser_helper.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace parser {
		template<typename CharT>
		constexpr auto find_first_of( daw::basic_string_view<CharT> const str,
		                              CharT const value ) noexcept {
			auto it = str.cbegin( );
			while( it != str.cend( ) and *it != value ) {
				++it;
			}
			return it;
		}

		template<typename CharT, typename Predicate>
		constexpr auto find_first_of_when(
		  daw::basic_string_view<CharT> str,
		  Predicate pred ) noexcept( noexcept( pred( CharT{ } ) ) ) {
			auto it = str.cbegin( );
			while( it != str.cend( ) and !pred( *it ) ) {
				++it;
			}
			return it;
		}

		template<typename CharT>
		constexpr daw::basic_string_view<CharT>
		trim_left( daw::basic_string_view<CharT> str ) noexcept {
			while( !str.empty( ) and is_unicode_whitespace( str.front( ) ) ) {
				str.remove_prefix( );
			}
			return str;
		}

		template<typename CharT>
		constexpr daw::basic_string_view<CharT>
		trim_right( daw::basic_string_view<CharT> str ) noexcept {
			while( !str.empty( ) and is_unicode_whitespace( str.back( ) ) ) {
				str.remove_suffix( );
			}
			return str;
		}

		template<typename CharT>
		constexpr daw::basic_string_view<CharT>
		trim( daw::basic_string_view<CharT> str ) noexcept {
			return trim_right( trim_left( str ) );
		}

		template<typename Int, typename CharT>
		constexpr Int parse_unsigned_int( daw::basic_string_view<CharT> str ) {
			Int i = 0;
			daw::parser::parse_unsigned_int( str.cbegin( ), str.cend( ), i );
			return i;
		}

		template<typename Int, typename CharT>
		constexpr Int parse_int( daw::basic_string_view<CharT> str ) {
			Int i = 0;
			daw::parser::parse_int( str.cbegin( ), str.cend( ), i );
			return i;
		}

		template<char... vals>
		class char_in_t {
			template<char check_val, char... check_vals>
			constexpr bool check( char const c ) const noexcept {
				return ( c == check_val ) or ( ( c == check_vals ) or ... );
			}

		public:
			constexpr bool operator( )( char const c ) const noexcept {
				return check<vals...>( c );
			}
		};
	} // namespace parser
} // namespace daw
