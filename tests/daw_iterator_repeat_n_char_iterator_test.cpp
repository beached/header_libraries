// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/iterator/daw_repeat_n_char_iterator.h"

#include "daw/daw_benchmark.h"

#include <cstddef>
#include <iostream>

int main( ) {
	daw::bench_n_test<100'000>( "append string( n, ' ' )", []( ) {
		std::string sm = "This is a test";
		daw::do_not_optimize( sm );
		sm.append( std::string( 16, ' ' ) );
		daw::do_not_optimize( sm );
	} );

	daw::bench_n_test<100'000>( "append string( first, last )", []( ) {
		std::string sm = "This is a test";
		daw::do_not_optimize( sm );
		sm.append( daw::repeat_n_char_iterator( 16, ' ' ),
		           daw::repeat_n_char_end( ) );
		daw::do_not_optimize( sm );
	} );

	daw::bench_n_test<100'000>( "repeated(100) append string( n, ' ' )", []( ) {
		std::string sm = "This is a test";
		daw::do_not_optimize( sm );
		for( size_t n = 0; n < 100; ++n ) {
			sm.append( std::string( 16, ' ' ) );
		}
		daw::do_not_optimize( sm );
	} );

	daw::bench_n_test<100'000>(
	  "repeated(100) append string( first, last )", []( ) {
		  std::string sm = "This is a test";
		  daw::do_not_optimize( sm );
		  for( size_t n = 0; n < 100; ++n ) {
			  sm.append( daw::repeat_n_char_iterator( 16, ' ' ),
			             daw::repeat_n_char_end( ) );
		  }
		  daw::do_not_optimize( sm );
	  } );

	daw::bench_n_test<100'000>( "e repeated(100) append string( n, ' ' )", []( ) {
		std::string sm{ };
		daw::do_not_optimize( sm );
		for( size_t n = 0; n < 100; ++n ) {
			sm.append( std::string( 16, ' ' ) );
		}
		daw::do_not_optimize( sm );
	} );

	daw::bench_n_test<100'000>(
	  "e repeated(100) append string( first, last )", []( ) {
		  std::string sm{ };
		  daw::do_not_optimize( sm );
		  for( size_t n = 0; n < 100; ++n ) {
			  sm.append( daw::repeat_n_char_iterator( 16, ' ' ),
			             daw::repeat_n_char_end( ) );
		  }
		  daw::do_not_optimize( sm );
	  } );

	daw::bench_n_test<100'000>( "repeated(100) append string( n, ' ' )", []( ) {
		std::string sm =
		  "This is a "
		  "tesdfkjsdflksjdflksdjfsdlkfjsdlfkjsdflksdjflsdkfjsdlkfjsdlfkjsdflksdjfsl"
		  "dkfjsdlkfjsdflksjflksdjfsdlkfjdsflksjdfldkjt";
		daw::do_not_optimize( sm );
		for( size_t n = 0; n < 100; ++n ) {
			sm.append( std::string( 16, ' ' ) );
		}
		daw::do_not_optimize( sm );
	} );

	daw::bench_n_test<100'000>(
	  "lrg repeated(100) append string( first, last )", []( ) {
		  std::string sm =
		    "This is a "
		    "tesdfkjsdflksjdflksdjfsdlkfjsdlfkjsdflksdjflsdkfjsdlkfjsdlfkjsdflksdjf"
		    "sldkfjsdlkfjsdflksjflksdjfsdlkfjdsflksjdfldkjt";
		  daw::do_not_optimize( sm );
		  for( size_t n = 0; n < 100; ++n ) {
			  sm.append( daw::repeat_n_char_iterator( 16, ' ' ),
			             daw::repeat_n_char_end( ) );
		  }
		  daw::do_not_optimize( sm );
	  } );
}
