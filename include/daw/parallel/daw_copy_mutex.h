// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//
//
#pragma once

#include <ciso646>
#include <memory>

/// This abstracts a mutex but allows for the owning class to be copied.
namespace daw {
	template<typename Mutex>
	class alignas( alignof( Mutex ) ) copiable_mutex {
		std::unique_ptr<Mutex> m_mutex;

	public:
		copiable_mutex( ) noexcept
		  : m_mutex( std::make_unique<Mutex>( ) ) {}

		copiable_mutex( copiable_mutex const & ) noexcept
		  : m_mutex( std::make_unique<Mutex>( ) ) {}

		copiable_mutex &operator=( copiable_mutex const & ) {
			// The caller should ensure locks are held if needed
			// m_mutex.reset( std::make_unique<Mutex>( ) );
			return *this;
		}

		copiable_mutex( copiable_mutex && ) noexcept = default;
		copiable_mutex &operator=( copiable_mutex && ) noexcept = default;
		~copiable_mutex( ) = default;

		void lock( ) {
			m_mutex->lock( );
		}

		[[nodiscard]] bool try_lock( ) {
			return m_mutex->try_lock( );
		}

		void unlock( ) {
			return m_mutex->unlock( );
		}

		[[nodiscard]] decltype( auto ) native_handle( ) {
			return m_mutex->native_handle( );
		}

		operator Mutex const &( ) const noexcept {
			return *m_mutex;
		}

		operator Mutex &( ) noexcept {
			return *m_mutex;
		}
	};
} // namespace daw
