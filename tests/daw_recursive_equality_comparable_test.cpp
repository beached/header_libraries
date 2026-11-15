// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_recursive_equality_comparable.h>

#include <vector>

namespace {
	struct equality_comparable {
		friend bool operator==( equality_comparable const &,
		                        equality_comparable const & ) = default;
	};

	struct not_equality_comparable {};

	static_assert( daw::is_recursively_equality_comparable_v<int> );
	static_assert(
	  daw::is_recursively_equality_comparable_v<equality_comparable> );
	static_assert(
	  not daw::is_recursively_equality_comparable_v<not_equality_comparable> );
	static_assert(
	  daw::is_recursively_equality_comparable_v<std::vector<std::vector<int>>> );
	static_assert( not daw::is_recursively_equality_comparable_v<
	               std::vector<std::vector<not_equality_comparable>>> );
} // namespace

int main( ) {}
