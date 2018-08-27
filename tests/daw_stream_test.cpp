// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <string>
#include <iostream>

#include "daw/boost_test.h"
#include "daw/daw_stream.h"

BOOST_AUTO_TEST_CASE( daw_stream_001 ) {
	using namespace std::string_literals;
	daw::con_out << "Hello the number is: " << 55 << ' ' << 1.3434343434L << ' '
	             << -234432 << ' ' << true << " string literal\n"s;
}

BOOST_AUTO_TEST_CASE( daw_memory_stream_001 ) {
	using namespace std::string_literals;
	char buffer[100] = {0};
	auto buff_os = daw::make_memory_buffer_stream( buffer, 100 );


	buff_os << "Hello the number is: " << 55 << ' ' << 1.3434343434L << ' '
							 << -234432 << ' ' << true << " string literal\n"s;

	std::cout << "wrote " << buff_os.get_underlying_stream().size( ) << " chars to buffer\n";
}

template<typename CharT>
constexpr size_t fill_buffer( CharT * buffer, size_t capacity ) {
	auto buff_os = daw::make_memory_buffer_stream( buffer, capacity );

	buff_os << "Hello the number is: " << 55 << ' ' << ' '
							 << -234432 << ' ' << true << " string literal\n";

	return buff_os.get_underlying_stream().size( );
}

template<size_t N>
struct str_t {
	char buffer[N] = {0};
	size_t sz;

	constexpr str_t( ): sz( fill_buffer( buffer, N ) ) {}
};

BOOST_AUTO_TEST_CASE( daw_memory_stream_002 ) {
	constexpr str_t<75> str{ };

	std::cout << daw::string_view( str.buffer, str.sz ) << '\n';
	std::operator<<( std::cout, "\n" );
}
