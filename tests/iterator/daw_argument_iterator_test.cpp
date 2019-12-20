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

#include "daw/daw_benchmark.h"
#include "daw/iterator/daw_argument_iterator.h"

#include <cstddef>
#include <numeric>
#include <string_view>

void test_end_001( ) {
	constexpr auto last = daw::arg_iterator_t( );
	daw::expecting( !last );
}

void test_range_001( ) {
	constexpr char const *argv_[] = {"test", "this", "out"};
	auto const args = daw::arg_iterator_t( 3, argv_ );
	daw::expecting( 3U, args.size( ) );
	daw::expecting( 0U, args.position( ) );
}

void test_prefix_inc_001( ) {
	constexpr char const *argv_[] = {"test", "this", "out"};
	auto args = daw::arg_iterator_t( 3, argv_ );
	++args;
	daw::expecting( 3U, args.size( ) );
	daw::expecting( 1U, args.position( ) );
	daw::expecting( args != args.end( ) );
}

void test_postfix_inc_001( ) {
	constexpr char const *argv_[] = {"test", "this", "out"};
	auto args = daw::arg_iterator_t( 3, argv_ );
	auto old_args = args++;
	daw::expecting( 3U, args.size( ) );
	daw::expecting( 1U, args.position( ) );

	daw::expecting( 3U, old_args.size( ) );
	daw::expecting( 0U, old_args.position( ) );
	daw::expecting( args != old_args );
	daw::expecting( args != args.end( ) );
	daw::expecting( old_args != old_args.end( ) );
}

void test_accumulate_001( ) {
	constexpr char const *argv_[] = {"test", "this", "out"};
	auto const args = daw::arg_iterator_t( 3, argv_ );

	auto result = std::accumulate( args.begin( ), args.end( ), 0,
	                               []( int i, auto &&sv ) { return i + sv[0]; } );
	daw::expecting( result, 't' + 't' + 'o' );
}

void test_index_001( ) {
	constexpr char const *argv_[] = {"test", "this", "out"};
	auto args = daw::arg_iterator_t( 3, argv_ );

	int sum = 0;
	for( int n = 0; n < static_cast<int>( args.size( ) ); ++n ) {
		sum += args[n][0];
	}
	daw::expecting( sum, 't' + 't' + 'o' );
}

int main( ) {
	test_end_001( );
	test_range_001( );
	test_prefix_inc_001( );
	test_postfix_inc_001( );
	test_accumulate_001( );
	test_index_001( );
}
