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
	bool has_error = false;
	daw::contract_failure_handler = [&] {
		// This is not a good idea in real code.  Do not call accessors if used
		has_error = true;
	};

	{
		auto v1 = pos_int{ -1 };
		daw_ensure( has_error = true );
	}
	daw::contract_failure_handler = daw::default_contract_failure;
}
