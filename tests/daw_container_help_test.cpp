// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#ifndef WIN32
#include <daw/daw_container_help.h>
#include <daw/daw_ensure.h>

#include <cstring>
#include <vector>
#endif

int main( ) {
#ifndef WIN32
	{
		auto foo = std::vector{ 1, 2, 3 };
		auto const foo_cap = foo.capacity( );
		auto const foo_size = foo.size( );
		daw_ensure( foo_size == 3 );
		auto foo_data = daw::extract_from_container( foo );
		daw_ensure( foo.empty( ) );
		daw_ensure( foo_data.size( ) == foo_size );
		daw_ensure( foo_cap == foo_data.capacity( ) );
		daw_ensure( foo_data.data( )[0] == 1 );
		daw_ensure( foo_data.data( )[1] == 2 );
		daw_ensure( foo_data.data( )[2] == 3 );

		constexpr auto vec_size = sizeof( std::vector<int> );
		auto const empty_vector = std::vector<int>{ };
		daw_ensure( memcmp( std::addressof( foo ), std::addressof( empty_vector ),
		                    vec_size ) == 0 );
	}
	{
		int *buff = new int[1024]{ 1, 2, 3 };
		auto vec = std::vector<int, std::allocator<int>>{ };
		daw::insert_into_container( vec, buff, std::allocator<int>{ }, 1024, 3 );
		daw_ensure( vec.size( ) == 3 );
		daw_ensure( vec.capacity( ) == 1024 );
		daw_ensure( vec.data( ) == buff );
		daw_ensure( vec == std::vector{ 1, 2, 3 } );
	}
#endif
}
