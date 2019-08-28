// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

#include "daw/daw_benchmark.h"
#include "daw/iterator/daw_repeat_n_char_iterator.h"

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
		std::string sm{};
		daw::do_not_optimize( sm );
		for( size_t n = 0; n < 100; ++n ) {
			sm.append( std::string( 16, ' ' ) );
		}
		daw::do_not_optimize( sm );
	} );

	daw::bench_n_test<100'000>(
	  "e repeated(100) append string( first, last )", []( ) {
		  std::string sm{};
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
