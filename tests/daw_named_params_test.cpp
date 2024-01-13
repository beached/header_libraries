// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_named_params.h>

#include <optional>
#include <string>

inline constexpr auto foo =
  DAW_MAKE_NAMED_OPTION( std::optional<int>, foo, std::nullopt );
inline auto bar =
  DAW_MAKE_NAMED_OPTION( std::optional<std::string>, bar, std::nullopt );

template<daw::nparam::Options... Args>
int func( Args &&...args ) {
	auto f = static_cast<bool>( daw::nparam::get_opt( foo, args... ) );
	auto b = static_cast<bool>( daw::nparam::get_opt( bar, args... ) );
	return not( f and b );
}

int main( ) {
	return func( foo = 5, bar = "Hello World" );
}
