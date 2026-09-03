// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>

#include <cstdlib>
#include <exception>

namespace {
	constexpr bool constant_evaluation_success( ) {
		daw_ensure( true );
		return true;
	}

	static_assert( constant_evaluation_success( ) );

	DAW_ATTRIB_NOINLINE void runtime_check( bool condition ) {
		daw_ensure( condition );
	}

	void optimization_time_success( ) {
		auto value = 42;
		daw_ensure( value == 42 );
	}
} // namespace

int main( int argc, char ** ) {
	if( argc > 1 ) {
		std::set_terminate( []( ) {
			std::_Exit( EXIT_SUCCESS );
		} );
		runtime_check( false );
		return EXIT_FAILURE;
	}

	runtime_check( argc == 1 );
	optimization_time_success( );
}
