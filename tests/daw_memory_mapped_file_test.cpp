// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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

#include "daw/daw_memory_mapped_file.h"

#include <cstdint>
#include <fstream>
#include <type_traits>

template<typename String>
void create_file( String &&str ) {
	std::ofstream fs{std::forward<String>( str )};
	if( !fs ) {
		return;
	}
	fs << "This is a test\n";
}

void daw_memory_mapped_file_001( std::string const &file_name ) {
	create_file( file_name );
	daw::filesystem::memory_mapped_file_t<std::uint8_t> test( static_cast<std::string_view>( file_name ) );
}

int main( ) {
	(void)daw_memory_mapped_file_001( "./blah.txt" );
}
