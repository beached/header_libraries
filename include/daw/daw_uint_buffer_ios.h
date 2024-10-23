// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_uint_buffer.h"

#include <cstdint>
#include <iostream>

namespace daw {
	std::ostream &operator<<( std::ostream &os, UInt8 rhs ) {
		os << static_cast<std::uint32_t>( rhs );
		return os;
	}

	std::ostream &operator<<( std::ostream &os, UInt16 rhs ) {
		os << static_cast<std::uint32_t>( rhs );
		return os;
	}

	std::ostream &operator<<( std::ostream &os, UInt32 rhs ) {
		os << static_cast<std::uint32_t>( rhs );
		return os;
	}

	std::ostream &operator<<( std::ostream &os, UInt64 rhs ) {
		os << static_cast<std::uint64_t>( rhs );
		return os;
	}
} // namespace daw
