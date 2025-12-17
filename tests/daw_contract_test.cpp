// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_do_not_optimize.h"

#include <daw/daw_contract.h>
#include <daw/daw_ensure.h>

struct positive_integral {
	positive_integral( ) = default;

	template<typename T>
	constexpr bool operator( )( T const & value ) const {
		return value >= T{};
	}
};

int main( ) {
	using pos_int = daw::contract<int, positive_integral>;
	static constexpr auto v0 = pos_int( 42 );
	constexpr int const &v0cr = v0;
	(void)v0cr;
#if defined( DAW_USE_EXCEPTIONS )
	bool has_error = false;
	try {
		int x = -1;
		daw::do_not_optimize( x );
		auto v1 = pos_int{ x };
		(void)v1;
	} catch( daw::daw_contract_violation const & ) { has_error = true; }
	daw_ensure( has_error );
#endif
}
