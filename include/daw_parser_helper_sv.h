// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_parser_addons.h"
#include "daw_parser_helper.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace parser {
		template<typename CharT, typename TraitsT>
		constexpr auto find_first_of( daw::basic_string_view<CharT, TraitsT> const str, CharT const value ) noexcept {
			auto it = str.cbegin( );
			while( it != str.cend( ) && *it != value ) {
				++it;
			}
			return it;
		}

		template<typename CharT, typename TraitsT, typename Predicate>
		constexpr auto find_first_of_when( daw::basic_string_view<CharT, TraitsT> str,
		                                   Predicate pred ) noexcept( noexcept( pred( CharT{} ) ) ) {
			auto it = str.cbegin( );
			while( it != str.cend( ) && !pred( *it ) ) {
				++it;
			}
			return it;
		}

		template<typename CharT, typename TraitsT>
		constexpr daw::basic_string_view<CharT, TraitsT> trim_left( daw::basic_string_view<CharT, TraitsT> str ) noexcept {
			while( !str.empty( ) && is_unicode_whitespace( str.front( ) ) ) {
				str.remove_prefix( );
			}
			return str;
		}

		template<typename CharT, typename TraitsT>
		constexpr daw::basic_string_view<CharT, TraitsT> trim_right( daw::basic_string_view<CharT, TraitsT> str ) noexcept {
			while( !str.empty( ) && is_unicode_whitespace( str.back( ) ) ) {
				str.remove_suffix( );
			}
			return str;
		}

		template<typename CharT, typename TraitsT>
		constexpr daw::basic_string_view<CharT, TraitsT> trim( daw::basic_string_view<CharT, TraitsT> str ) noexcept {
			return trim_right( trim_left( str ) );
		}

		template<typename CharT, typename TraitsT, typename Int>
		constexpr void parse_unsigned_int( daw::basic_string_view<CharT, TraitsT> str, Int & i ) {
			daw::parser::parse_unsigned_int( str.cbegin( ), str.cend( ), i );
		}

		template<typename CharT, typename TraitsT, typename Int>
		constexpr void parse_int( daw::basic_string_view<CharT, TraitsT> str, Int & i ) {
			daw::parser::parse_int( str.cbegin( ), str.cend( ), i );
		}
	} // namespace parser
} // namespace daw
