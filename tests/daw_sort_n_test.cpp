
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

#include "daw/daw_benchmark.h"
#include "daw/daw_random.h"
#include "daw/daw_sort_n.h"

int main( ) {
	auto const random32 = daw::make_random_data<size_t>( 32 );

	{
		auto a = random32;
		daw::expecting( !std::is_sorted( begin( a ), end( a ) ) );
		daw::bench_n_test<100'000>( "Standard Sort", [a]( ) mutable {
			daw::do_not_optimize( a );
			std::sort( begin( a ), end( a ) );
			daw::do_not_optimize( a );
		} );
	}

	{
		auto a = random32;
		daw::expecting( !std::is_sorted( begin( a ), end( a ) ) );
		daw::bench_n_test<100'000>( "sort_32 Sort", [a]( ) mutable {
			daw::do_not_optimize( a );
			daw::sort_32( begin( a ), end( a ) );
			daw::do_not_optimize( a );
		} );
	}
}
