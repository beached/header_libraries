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
#include <string>
#include <vector>

#include "daw/daw_benchmark.h"
#include "daw/daw_string_fmt.h"

void string_fmt_test_001( ) {
	std::cout << daw::string_fmt::v1::fmt(
	  "This is a {0} of the {1} and has been used {2} times for {0}ing\n", "test",
	  "daw::string_fmt::v1::fmt", 1'000'000 );
}

void string_fmt_test_002( ) {
	daw::string_fmt::v1::fmt_t f{
	  "This is a {0} of the {1} and has been used {2} times for {0}ing\n"};
	std::cout << f( "test", "daw::string_fmt::v1::fmt", 1'000'000 );
}

void string_fmt_test_single_item_001( ) {
	auto result = daw::string_fmt::v1::fmt( "{0}", 5 );

	daw::expecting( result, "5" );
}

void string_fmt_test_recursion_001( ) {
	auto result =
	  daw::string_fmt::v1::fmt( "{0}", daw::string_fmt::v1::fmt( "{0}", 5 ) );
	daw::expecting( result, "5" );
	// auto const ans1 = daw::bench_test( "string_fmt_test_001", [&]( ) {
	// test_func( ) } ); daw::expecting( *ans1, );
}

void string_fmt_test_out_of_range_001( ) {
	daw::expecting_exception<daw::string_fmt::v1::invalid_string_fmt_index>(
	  []( ) { daw::string_fmt::v1::fmt( "{1}", 5 ); } );
}

void string_fmt_perf_001( ) {
	std::cout << "Larger format perf\n";
	daw::bench_test( "string_fmt perf", [&]( ) {
		for( size_t n = 0; n < 1'000'000; ++n ) {
			auto tst = daw::string_fmt::v1::fmt(
			  "This is a {0} of the {1} and has been used {2} times for {0}ing\n",
			  "test", "daw::string_fmt::v1::fmt", n );
			daw::do_not_optimize( tst );
		}
	} );

	daw::bench_test( "fmt_t perf", [&]( ) {
		auto const formatter = daw::string_fmt::v1::fmt_t{
		  "This is a {0} of the {1} and has been used {2} times for {0}ing\n"};
		for( size_t n = 0; n < 1'000'000; ++n ) {
			auto tst = formatter( "test", "daw::string_fmt::v1::fmt", n );
			daw::do_not_optimize( tst );
		}
	} );

	daw::bench_test( "string_concat perf", [&]( ) {
		using std::to_string;
		using daw::string_fmt::v1::sf_impl::to_string;
		for( size_t n = 0; n < 1'000'000; ++n ) {
			auto tst = "This is a" + to_string( "test" ) + " of the " +
			           to_string( "daw::string_fmt::v1::fmt" ) +
			           " and has been used " + to_string( n ) + " times for " +
			           to_string( "test" ) + "ing\n";
			daw::do_not_optimize( tst );
		}
	} );
}

void string_fmt_perf_002( ) {
	std::cout << "\n\nSmaller format perf\n";
	daw::bench_test( "string_fmt perf", [&]( ) {
		for( size_t n = 0; n < 1'000'000; ++n ) {
			auto tst = daw::string_fmt::v1::fmt(
			  "This is a test of the daw::string_fmt::v1::fmt and has been used "
			  "{2} "
			  "times for "
			  "testing\n",
			  "test", "daw::string_fmt::v1::fmt", n );
			daw::do_not_optimize( tst );
		}
	} );

	daw::bench_test( "fmt_t perf", [&]( ) {
		auto const formatter = daw::string_fmt::v1::fmt_t{
		  "This is a test of the daw::string_fmt::v1::fmt and has been used {2} "
		  "times for "
		  "testing\n"};
		for( size_t n = 0; n < 1'000'000; ++n ) {
			auto tst = formatter( n );
			daw::do_not_optimize( tst );
		}
	} );

	daw::bench_test( "string_concat perf", [&]( ) {
		using std::to_string;
		using daw::string_fmt::v1::sf_impl::to_string;
		for( size_t n = 0; n < 1'000'000; ++n ) {
			auto tst =
			  "This is a test of the daw::string_fmt::v1::fmt and has been used " +
			  to_string( n ) + " times for testing\n";
			daw::do_not_optimize( tst );
		}
	} );
}

void string_fmt_has_to_string_001( ) {
	struct A {
		int a;
		explicit operator std::string( ) const {
			return std::to_string( a );
		}
	};
	A a{1};
	auto result = daw::string_fmt::v1::fmt( "Testing {0}", a );

	daw::expecting( result, "Testing 1" );
}

int main( ) {
	string_fmt_test_001( );
	string_fmt_test_002( );
	string_fmt_test_single_item_001( );
	string_fmt_test_recursion_001( );
	string_fmt_test_out_of_range_001( );
	string_fmt_perf_001( );
	string_fmt_perf_002( );
	string_fmt_has_to_string_001( );
}
