// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

	public:
		concurrent_queue( )
		  : m_queue( )
		  , m_condition( )
		  , m_forced_exit( false ) {}

		void push( Data const &data ) {
			std::unique_lock<std::mutex> lock( m_mutex );
			m_queue.push( data );
			lock.unlock( );
			m_condition.notify_one( );
		}

		bool empty( ) const {
			std::unique_lock<std::mutex> lock( m_mutex );
			return m_queue.empty( );
		}

		bool try_pop( Data &popped_value ) {
			std::unique_lock<std::mutex> lock( m_mutex );
			if( m_queue.empty( ) ) {
				return false;
			}

			popped_value = m_queue.front( );
			m_queue.pop( );
			return true;
		}

		void wait_and_pop( Data &popped_value ) {
			std::unique_lock<std::mutex> lock( m_mutex );
			while( m_queue.empty( ) ) {
				m_condition.wait( lock );
				if( m_forced_exit ) {
					return;
				}
			}

			popped_value = m_queue.front( );
			m_queue.pop( );
		}

		void reset( ) {
			m_forced_exit = false;
		}

		void exit_all( ) {
			m_forced_exit = true;
			m_condition.notify_all( );
		}
	}; // class concurrent_queue
} // namespace daw
