// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <atomic>

namespace daw {
	class allow_once_t {
		mutable std::atomic_bool m_called = false;

	public:
		explicit allow_once_t( ) = default;

		void reset( ) {
			m_called = false;
		}

		bool check( ) const {
			return m_called.exchange( true, std::memory_order_acq_rel );
		}

		explicit operator bool( ) const {
			return check( );
		}
	};
} // namespace daw
