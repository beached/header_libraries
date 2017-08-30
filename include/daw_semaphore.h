// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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

namespace daw {
	template<typename Mutex, typename ConditionVariable>
	class basic_semaphore {
		std::unique_ptr<Mutex> m_mutex;
		std::unique_ptr<ConditionVariable> m_condition;
		intmax_t m_count;

	  public:
		explicit basic_semaphore( intmax_t count = 0 )
		    : m_mutex{std::make_unique<Mutex>( )}
		    , m_condition{std::make_unique<ConditionVariable>( )}
		    , m_count{count} {}

		~basic_semaphore( ) = default;
		basic_semaphore( basic_semaphore const & ) = delete;
		basic_semaphore( basic_semaphore && ) noexcept = default;
		basic_semaphore &operator=( basic_semaphore const & ) = delete;
		basic_semaphore &operator=( basic_semaphore && ) noexcept = default;

		void notify( ) {
			std::unique_lock<std::mutex> lock{*m_mutex};
			++m_count;
			m_condition->notify_one( );
		}

		void wait( ) {
			std::unique_lock<std::mutex> lock{*m_mutex};
			m_condition->wait( lock, [&]( ) { return m_count > 0; } );
			--m_count;
		}

		bool try_wait( ) {
			std::unique_lock<std::mutex> lock{*m_mutex};
			if( m_count > 0 ) {
				--m_count;
				return true;
			}
			return false;
		}

		template<typename Rep, typename Period>
		auto wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			std::unique_lock<std::mutex> lock{*m_mutex};
			auto status = m_condition->wait_for( lock, rel_time, [&]( ) { return m_count > 0; } );
			if( status ) {
				--m_count;
			}
			return status;
		}

		template<typename Rep, typename Period>
		auto wait_until( std::chrono::duration<Rep, Period> const &rel_time ) {
			std::unique_lock<std::mutex> lock{*m_mutex};
			auto status = m_condition->wait_until( lock, rel_time, [&]( ) { return m_count > 0; } );
			if( status ) {
				--m_count;
			}
			return status;
		}
	}; // basic_semaphore

	using semaphore = basic_semaphore<std::mutex, std::condition_variable>;
	
	template<typename Mutex, typename ConditionVariable>
	class basic_shared_semaphore {
		std::shared_ptr<basic_semaphore<Mutex, ConditionVariable>> m_semaphore;
	public:
	  explicit basic_shared_semaphore( intmax_t count = 0 )
		  : m_semaphore{std::make_shared<basic_semaphore<Mutex, ConditionVariable>>( count )} {}

	  explicit basic_shared_semaphore( basic_semaphore<Mutex, ConditionVariable> &&semaphore )
		  : m_semaphore{std::make_shared<basic_semaphore<Mutex, ConditionVariable>>( std::move( semaphore ) )} {}

	  ~basic_shared_semaphore( ) = default;
	  basic_shared_semaphore( basic_shared_semaphore const & ) = default;
	  ;
	  basic_shared_semaphore( basic_shared_semaphore && ) noexcept = default;
	  basic_shared_semaphore &operator=( basic_shared_semaphore const & ) = default;
	  basic_shared_semaphore &operator=( basic_shared_semaphore && ) noexcept = default;

	  void notify( ) {
		  m_semaphore->notify( );
		}

		void wait( ) {
			m_semaphore->wait( );
		}

		bool try_wait( ) {
			return m_semaphore->try_wait( );
		}

		template<typename Rep, typename Period>
		auto wait_for( std::chrono::duration<Rep, Period> const &rel_time ) {
			return m_semaphore->wait_for( rel_time );
		}

		template<typename Rep, typename Period>
		auto wait_until( std::chrono::duration<Rep, Period> const &rel_time ) {
			return m_semaphore->wait_until( rel_time );
		}
	};	// basic_shared_semaphore

	using shared_semaphore = basic_shared_semaphore<std::mutex, std::condition_variable>;

	template<typename Mutex, typename ConditionVariable>
	void wait_all( std::initializer_list<basic_semaphore<Mutex, ConditionVariable>> semaphores ) {
		for( auto & sem: semaphores ) {
			sem->wait( );
		}
	}
} // namespace daw
