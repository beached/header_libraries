// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <atomic>
#include <immintrin.h>

namespace daw {
	class spin_lock {
		std::atomic_flag m_flag = ATOMIC_FLAG_INIT;

	public:
		inline bool try_lock( ) noexcept {
			return not m_flag.test_and_set( );
		}

		inline void lock( ) noexcept {
			while( not try_lock( ) ) {
				_mm_pause( );
			}
		}

		inline void unlock( ) noexcept {
			m_flag.clear( std::memory_order_release );
		}
	};
} // namespace daw
