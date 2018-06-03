// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>

#include "daw_exception.h"

namespace daw {
	template<typename Mutex, typename ConditionVariable>
	class basic_counting_semaphore {
		// These are in unique_ptr's so that the semaphore can be moved
		std::unique_ptr<Mutex> m_mutex;
		std::unique_ptr<ConditionVariable> m_condition;
		intmax_t m_count;

		auto stop_waiting( ) const {
			return [&]( ) { return m_count <= 0; };
		}

	public:
		basic_counting_semaphore( )
		  : m_mutex( )
		  , m_condition( )
		  , m_count( 1 ) {}

		template<typename Int>
		explicit basic_counting_semaphore( Int count )
		  : m_mutex( )
		  , m_condition( )
		  , m_count( static_cast<intmax_t>( count ) ) {

			daw::exception::DebugAssert( m_count >= 0, "Count cannot be negative" );
		}

		template<typename Int>
		basic_counting_semaphore( Int count, bool latched )
		  : m_mutex( )
		  , m_condition( )
		  , m_count( static_cast<intmax_t>( count ) ) {

			daw::exception::DebugAssert( m_count >= 0, "Count cannot be negative" );
		}

		void reset( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			m_count = 1;
		}

		template<typename Int>
		void reset( Int count ) {
			daw::exception::DebugAssert( m_count >= 0, "Count cannot be negative" );
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			m_count = static_cast<intmax_t>( count );
		}

		void notify( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			--m_count;
			daw::exception::DebugAssert( m_count >= 0,
			                             "Notify called too many times" );
			m_condition->notify_all( );
		}

		void notify_one( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			--m_count;
			daw::exception::DebugAssert( m_count >= 0,
			                             "Notify called too many times" );
			m_condition->notify_one( );
		}

		void wait( ) {
			for( size_t n = 0; n < 100; ++n ) {
				if( try_wait( ) ) {
					return;
				}
			}
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			m_condition->wait( lock, stop_waiting( ) );
		}

		bool try_wait( ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			return stop_waiting( )( );
		}

		template<typename Rep, typename Period>
		auto wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			return m_condition->wait_for( lock, rel_time, stop_waiting( ) );
		}

		template<typename Clock, typename Duration>
		auto
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			auto lock = std::unique_lock<Mutex>( *m_mutex );
			return m_condition->wait_until( lock, timeout_time, stop_waiting( ) );
		}
	}; // basic_counting_semaphore

	using counting_semaphore =
	  basic_counting_semaphore<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_shared_counting_semaphore {
		std::shared_ptr<basic_counting_semaphore<Mutex, ConditionVariable>>
		  counting_semaphore;

	public:
		basic_shared_counting_semaphore( )
		  : counting_semaphore(
		      std::make_shared<
		        basic_counting_semaphore<Mutex, ConditionVariable>>( ) ) {}

		template<typename Int>
		explicit basic_shared_counting_semaphore( Int count )
		  : counting_semaphore(
		      std::make_shared<basic_counting_semaphore<Mutex, ConditionVariable>>(
		        count ) ) {}

		template<typename Int>
		explicit basic_shared_counting_semaphore( Int count, bool latched )
		  : counting_semaphore(
		      std::make_shared<basic_counting_semaphore<Mutex, ConditionVariable>>(
		        count, latched ) ) {}

		explicit basic_shared_counting_semaphore(
		  basic_counting_semaphore<Mutex, ConditionVariable> &&sem )
		  : counting_semaphore(
		      std::make_shared<basic_counting_semaphore<Mutex, ConditionVariable>>(
		        std::move( sem ) ) ) {}

		void notify( ) {
			counting_semaphore->notify( );
		}

		void add_notifier( ) {
			counting_semaphore->add_notifier( );
		}

		void set_latch( ) {
			counting_semaphore->set_latch( );
		}

		void wait( ) {
			counting_semaphore->wait( );
		}

		bool try_wait( ) {
			return counting_semaphore->try_wait( );
		}

		template<typename Rep, typename Period>
		decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			return counting_semaphore->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		decltype( auto )
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			return counting_semaphore->wait_until( timeout_time );
		}
	}; // basic_shared_counting_semaphore

	using shared_counting_semaphore =
	  basic_shared_counting_semaphore<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	void wait_all(
	  std::initializer_list<basic_counting_semaphore<Mutex, ConditionVariable>>
	    semaphores ) {
		for( auto &sem : semaphores ) {
			sem->wait( );
		}
	}
} // namespace daw
