// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <memory>
#include <mutex>

namespace daw {
	// A ref counted mutex
	template<typename Mutex>
	class alignas( alignof( Mutex ) ) basic_shared_mutex {
		struct member_t {
			alignas( alignof( Mutex ) ) Mutex data = Mutex( );
		};
		std::shared_ptr<member_t> m_mutex = std::make_shared<member_t>( );

	public:
		basic_shared_mutex( ) noexcept(
		  std::is_nothrow_default_constructible_v<Mutex> ) {}

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

	using shared_mutex = basic_shared_mutex<std::mutex>;
} // namespace daw
