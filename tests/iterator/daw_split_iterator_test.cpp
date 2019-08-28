// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
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

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>

#include "daw/daw_benchmark.h"
#include "daw/iterator/daw_split_iterator.h"

void split_it_001( ) {
	std::string str = "This is a test of the split";
	auto it = daw::make_split_it( str, ' ' );

	while( it != str.end( ) ) {
		std::cout << *it << '\n';
		++it;
	}
}

void split_it_002( ) {
	std::string str = "t  j ";
	auto it = daw::make_split_it( str, ' ' );

	while( it != str.end( ) ) {
		std::cout << *it << '\n';
		++it;
	}
}

void split_it_003( ) {
	std::string str = "This is a test of the split";
	auto it = daw::make_split_it( str, ' ' );
	auto const last = it.make_end( );

	std::vector<std::string> results{};
	std::transform( it, last, std::back_inserter( results ),
	                []( auto sv ) { return sv.to_string( ); } );

	for( auto const &s : results ) {
		std::cout << s << '\n';
	}
}

void split_it_004( ) {
	std::string str = "This is a test of the split";
	auto it = daw::make_split_it( str, ' ' );
	auto const last = daw::split_it<char>{};

	while( it != last ) {
		auto sv = *it;
		++it;
		--it;
		auto sv_prev = *it;
		daw::expecting( sv, sv_prev );
		++it;
	}
}

int main( ) {
	split_it_001( );
	split_it_002( );
	split_it_003( );
	split_it_004( );
}
