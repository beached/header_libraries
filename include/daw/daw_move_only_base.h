// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

namespace daw {
	template<typename>
	struct move_only_base {
		move_only_base( ) = default;
		~move_only_base( ) = default;
		move_only_base( move_only_base && ) = default;
		move_only_base &operator=( move_only_base && ) = default;
		move_only_base( move_only_base const & ) = delete;
		move_only_base &operator=( move_only_base const & ) = delete;
	};
}


