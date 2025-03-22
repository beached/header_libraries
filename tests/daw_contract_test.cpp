// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_contract.h>
#include <daw/daw_ensure.h>

int main( ) {
	using positive_integral = decltype( []<typename T>( T const &value ) {
		return value >= T{ };
	} );
	using pos_int = daw::contract<int, positive_integral>;
	static constexpr auto v0 = pos_int( 42 );
	constexpr int const &v0cr = v0;
	(void)v0cr;
#if defined( DAW_USE_EXCEPTIONS )
	bool has_error = false;
	try {
		auto v1 = pos_int{ -1 };
		(void)v1;
	} catch( daw::daw_contract_violation const & ) { has_error = true; }
	daw_ensure( has_error );
#endif
}
