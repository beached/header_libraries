// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_bitset_helper.h"

#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_ensure.h>

#include <bitset>

int main( ) {
	DAW_CPP23_CX_BITSET auto const bs0 =
	  daw::create_bitset_from_set_positions<16>( { 1, 2, 5, 10 } );
	daw_ensure( bs0.count( ) == 4 );
	daw_ensure( bs0.test( 1 ) );
	daw_ensure( bs0.test( 2 ) );
	daw_ensure( bs0.test( 5 ) );
	daw_ensure( bs0.test( 10 ) );

	enum class EPos { a, b, c, d, e, f };
	DAW_CPP23_CX_BITSET auto const bs1 =
	  daw::create_bitset_from_set_positions<4>( { EPos::a, EPos::d } );
	daw_ensure( bs1.count( ) == 2 );
	daw_ensure( bs1.test( static_cast<unsigned>( EPos::a ) ) );
	daw_ensure( bs1.test( static_cast<unsigned>( EPos::d ) ) );
#if defined( DAW_HAS_CPP23_CONSTEXPR_BITSET )
	constexpr auto bs2 =
	  daw::create_bitset_from_set_positions<16>( { 1, 2, 5, 10 } );
	static_assert( bs2.count( ) == 4 );
	static_assert( bs2.test( 1 ) );
	static_assert( bs2.test( 2 ) );
	static_assert( bs2.test( 5 ) );
	static_assert( bs2.test( 10 ) );

	constexpr auto bs3 =
	  daw::create_bitset_from_set_positions<4>( { EPos::a, EPos::d } );
	static_assert( bs3.count( ) == 2 );
	static_assert( bs3.test( static_cast<unsigned>( EPos::a ) ) );
	static_assert( bs3.test( static_cast<unsigned>( EPos::d ) ) );
#endif
}
