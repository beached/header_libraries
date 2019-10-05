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

#include "daw_exception.h"
#include "daw_string_view.h"

namespace daw {
	inline std::basic_string<char> read_file( ::daw::basic_string_view<char> p ) {
		auto in_file = std::basic_ifstream<char>( p.to_string( ).c_str( ) );
		daw::exception::Assert( in_file, "Could not open file" );

		return std::string( std::istreambuf_iterator<char>{in_file}, {} );
	}

#if __has_include( <filesystem> ) and defined( __cpp_lib_filesystem )
	inline std::basic_string<wchar_t>
	read_file( ::daw::basic_string_view<wchar_t> p ) {
		auto in_file = std::basic_ifstream<wchar_t>( p.to_string( ).c_str( ) );
		daw::exception::Assert( in_file, "Could not open file" );

		return std::wstring( std::istreambuf_iterator<wchar_t>{in_file}, {} );
	}
#endif
} // namespace daw
