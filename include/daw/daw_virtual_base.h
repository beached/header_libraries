// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw {
	template<typename /*CRTP*/>
	struct virtual_base {
		virtual_base( ) = default;

		virtual_base( virtual_base const & ) = default;
		virtual_base( virtual_base && ) = default;
		virtual_base &operator=( virtual_base const & ) = default;
		virtual_base &operator=( virtual_base && ) = default;
		virtual ~virtual_base( ) = default;
	};
} // namespace daw
