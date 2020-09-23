// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <ciso646>
#include <memory>
#include <mutex>

namespace daw {
	// A ref counted mutex
	template<typename Mutex>
	class alignas( alignof( Mutex ) ) basic_unique_mutex {
		struct member_t {
			alignas( alignof( Mutex ) ) Mutex data = Mutex( );
		};
		std::unique_ptr<member_t> m_mutex = std::make_unique<member_t>( );

	public:
		basic_unique_mutex( ) noexcept {}

		void lock( ) {
			m_mutex->data.lock( );
		}

		[[nodiscard]] bool try_lock( ) {
			return m_mutex->data.try_lock( );
		}

		void unlock( ) {
			m_mutex->data.unlock( );
		}

		Mutex &get( ) noexcept {
			return m_mutex->data;
		}

		Mutex const &get( ) const noexcept {
			return m_mutex->data;
		}
	};

	using unique_mutex = basic_unique_mutex<std::mutex>;
} // namespace daw
