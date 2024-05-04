// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_constant.h>

#include <type_traits>

int main( ) {
	using t = daw::constant<55>;
	static_assert( std::is_same_v<typename t::type, int> );

	auto c = daw::constant_v<55>;

	static_assert( std::is_same_v<t, decltype( c )> );
}
