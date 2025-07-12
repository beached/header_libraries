// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"

#include "daw/daw_expected.h"

#include <iostream>

void daw_benchmark_test_001( ) {
	std::cout << "Time of: " << daw::benchmark( []( ) {
		std::cout << "Hello\n";
	} );
	daw::show_benchmark( 1, "timed", []( ) {
		std::cout << "Hello\n";
	} );
}

void daw_benchmark_test_002( ) {
	int x = 0;
	daw::do_not_optimize( x );
}

void daw_bench_test_test_001( ) {
	auto res = daw::bench_test(
	  "sqr: ",
	  []( auto i ) {
		  return i * i;
	  },
	  55 );
	daw::expecting( res.has_value( ) );
	daw::expecting( 3025, *res );
}

void daw_bench_n_test_test_001( ) {
	auto res = daw::bench_n_test<100>(
	  "sqr: ",
	  []( auto i ) {
		  return i * i;
	  },
	  55 );
	daw::expecting( res.has_value( ) );
	daw::expecting( 3025, *res );
}

int main( )
#if defined( DAW_USE_EXCEPTIONS )
  try
#endif
{
	daw_benchmark_test_001( );
	daw_benchmark_test_002( );
	daw_bench_test_test_001( );
	daw_bench_n_test_test_001( );
}
#if defined( DAW_USE_EXCEPTIONS )
catch( std::exception const &ex ) {
	std::cerr << "Uncaught Exception: " << ex.what( ) << '\n' << std::flush;
	return 1;
} catch( ... ) {
	std::cerr << "Unknown uncaught exception:\n" << std::flush;
	throw;
}
#endif