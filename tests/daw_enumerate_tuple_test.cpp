// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/algorithms/daw_enumerate_tuple.h"

#include <string_view>

int main( ) {
	using namespace std::literals;
	static constexpr auto x = std::tuple{"a"sv, "b"sv};
	static constexpr auto enum_x = daw::enumerate_tuple( x );
	static_assert( std::get<0>( enum_x ).index == 0 );
	static_assert( std::get<0>( enum_x ).value == "a"sv );
	static_assert( std::get<1>( enum_x ).index == 1 );
	static_assert( std::get<1>( enum_x ).value == "b"sv );

	static constexpr auto enum_x2 = daw::enumerate_tuple<int>( x );
	static_assert( std::get<0>( enum_x2 ).index == 0 );
	static_assert( std::get<0>( enum_x2 ).value == "a"sv );
	static_assert( std::get<1>( enum_x2 ).index == 1 );
	static_assert( std::get<1>( enum_x2 ).value == "b"sv );
	static_assert(
			std::same_as<decltype( std::get<0>( std::get<0>( enum_x2 ) ) ),
			             int> )
		;

	using elem0_t = decltype( std::get<
		0>( enum_x2 ) );
	static_assert(
		std::same_as<daw::enumerate_tuple_element_t<std::string_view const &, 0>,
		             elem0_t> );
	static_assert( std::tuple_size_v<elem0_t> == 2 );
	auto const [index,element] = std::get<0>( enum_x2 );
	static_assert( std::same_as<int, DAW_TYPEOF( index )> );
}