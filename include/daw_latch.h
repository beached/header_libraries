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

#include <atomic>
#include <cstdint>

#include "daw_condition_variable.h"
#include "daw_exception.h"
#include "daw_value_ptr.h"

namespace daw {
	template<typename Mutex, typename ConditionVariable>
	class basic_latch {
		// These are in value_ptr's so that the semaphore can be moved
		daw::condition_variable m_condition;
		value_ptr<std::atomic_intmax_t> m_count;

		auto stop_waiting( ) const {
			return [&]( ) { return static_cast<intmax_t>( *m_count ) <= 0; };
		}

		void decrement( ) {
			--( *m_count );
		}

	public:
		basic_latch( )
		  : m_condition( )
		  , m_count( 1 ) {}

		template<typename Int>
		explicit basic_latch( Int count )
		  : m_condition( )
		  , m_count( static_cast<intmax_t>( count ) ) {}

		template<typename Int>
		basic_latch( Int count, bool latched )
		  : m_condition( )
		  , m_count( static_cast<intmax_t>( count ) ) {}

		void reset( ) {
			m_count = 1;
		}

		template<typename Int>
		void reset( Int count ) {
			*m_count = static_cast<intmax_t>( count );
		}

		void notify( ) {
			decrement( );
			m_condition.notify_all( );
		}

		void notify_one( ) {
			decrement( );
			m_condition.notify_one( );
		}

		void wait( ) {
			for( size_t n = 0; n < 100; ++n ) {
				if( try_wait( ) ) {
					return;
				}
			}
			m_condition.wait( stop_waiting( ) );
		}

		bool try_wait( ) const {
			return stop_waiting( )( );
		}

		template<typename Rep, typename Period>
		auto wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			return m_condition.wait_for( rel_time, stop_waiting( ) );
		}

		template<typename Clock, typename Duration>
		auto
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			return m_condition.wait_until( timeout_time, stop_waiting( ) );
		}
	}; // basic_latch

	using latch = basic_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	class basic_shared_latch {
		std::shared_ptr<basic_latch<Mutex, ConditionVariable>> latch;

	public:
		basic_shared_latch( )
		  : latch( std::make_shared<basic_latch<Mutex, ConditionVariable>>( ) ) {}

		template<typename Int>
		explicit basic_shared_latch( Int count )
		  : latch(
		      std::make_shared<basic_latch<Mutex, ConditionVariable>>( count ) ) {}

		template<typename Int>
		explicit basic_shared_latch( Int count, bool latched )
		  : latch( std::make_shared<basic_latch<Mutex, ConditionVariable>>(
		      count, latched ) ) {}

		explicit basic_shared_latch( basic_latch<Mutex, ConditionVariable> &&sem )
		  : latch( std::make_shared<basic_latch<Mutex, ConditionVariable>>(
		      std::move( sem ) ) ) {}

		void notify( ) {
			latch->notify( );
		}

		void add_notifier( ) {
			latch->add_notifier( );
		}

		void set_latch( ) {
			latch->set_latch( );
		}

		void wait( ) {
			latch->wait( );
		}

		bool try_wait( ) const {
			return latch->try_wait( );
		}

		template<typename Rep, typename Period>
		decltype( auto )
		wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			return latch->wait_for( rel_time );
		}

		template<typename Clock, typename Duration>
		decltype( auto )
		wait_until( std::chrono::time_point<Clock, Duration> const &timeout_time ) {
			return latch->wait_until( timeout_time );
		}
	}; // basic_shared_latch

	using shared_latch = basic_shared_latch<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	void wait_all(
	  std::initializer_list<basic_latch<Mutex, ConditionVariable>> semaphores ) {
		for( auto &sem : semaphores ) {
			sem->wait( );
		}
	}
} // namespace daw
