// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <fstream>

#include "daw_string_view.h"

namespace daw {
	template<typename CharT>
	::std::optional<::std::basic_string<CharT>>
	read_file( ::daw::basic_string_view<CharT> p ) {
		auto in_file = ::std::basic_ifstream<CharT>( p.to_string( ).c_str( ) );
		if( not in_file ) {
			return {};
		}

		return ::std::basic_string<CharT>(
		  ::std::istreambuf_iterator<CharT>( in_file ), {} );
	}

	template<typename CharT>
	::std::optional<::std::basic_string<CharT>>
	read_file( ::std::basic_string_view<CharT> p ) {
		auto in_file = ::std::basic_ifstream<CharT>( p.to_string( ).c_str( ) );
		if( not in_file ) {
			return {};
		}

		return ::std::basic_string<CharT>(
		  ::std::istreambuf_iterator<CharT>( in_file ), {} );
	}

	template<typename CharT>
	::std::optional<::std::basic_string<CharT>> read_file( CharT const *str ) {
		return read_file( ::daw::basic_string_view<CharT>( str ) );
	}
} // namespace daw
