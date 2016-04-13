#pragma once

#include <deque>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <condition_variable>
#include <atomic>


using lock_t = std::unique_lock<std::mutex>;

class notification_queue {
	std::deque<std::function<void( )>> m_queue;
	bool m_done { false };
	std::mutex m_mutex;
	std::condition_variable m_ready;

public:
	bool try_pop( std::function<void( )>& x ) {
		lock_t lock { m_mutex, std::try_to_lock };
		if( !lock || m_queue.empty( ) ) {
			return false;
		}
		x = std::move( m_queue.front( ) );
		m_queue.pop_front( );
		return true;
	}

	template<typename F>
	bool try_push( F&& f ) {
		{
			lock_t lock { m_mutex, std::try_to_lock };
			if( !lock ) {
				return false;
			}
			m_queue.emplace_back( std::forward<F>( f ) );
		}
		m_ready.notify_one( );
		return true;
	}

	void done( ) {
		{
			std::unique_lock<std::mutex> lock { m_mutex };
			m_done = true;
		}
		m_ready.notify_all( );
	}

	bool pop( std::function<void( )>& x ) {
		lock_t lock { m_mutex };
		while( m_queue.empty( ) && !m_done ) {
			m_ready.wait( lock );
		}
		if( m_queue.empty( ) ) {
			return false;
		}
		x = std::move( m_queue.front( ) );
		m_queue.pop_front( );
		return true;
	}

	template<typename F>
	void push( F&& f ) {
		{
			lock_t lock { m_mutex };
			m_queue.emplace_back( std::forward<F>( f ) );
		}
		m_ready.notify_one( );
	}
};

/**************************************************************************************************/

class task_system {
	const size_t m_count { std::thread::hardware_concurrency( ) };
	std::vector<std::thread> m_threads;
	std::vector<notification_queue> m_queue { m_count };
	std::atomic<size_t> m_index { 0 };

	void run( unsigned i ) {
		while( true ) {
			std::function<void( )> f;

			for( unsigned n = 0; n != m_count * 32; ++n ) {
				if( m_queue[(i + n) % m_count].try_pop( f ) ) {
					break;
				}
			}
			if( !f && !m_queue[i].pop( f ) ) {
				break;
			}
			f( );
		}
	}

public:
	task_system( ) {
		for( unsigned n = 0; n != m_count; ++n ) {
			m_threads.emplace_back( [&, n] { run( n ); } );
		}
	}

	~task_system( ) {
		for( auto& e : m_queue ) {
			e.done( );
		}
		for( auto& e : m_threads ) {
			e.join( );
		}
	}

	template <typename F>
	void async_( F&& f ) {
		auto i = m_index++;

		for( unsigned n = 0; n != m_count; ++n ) {
			if( m_queue[(i + n) % m_count].try_push( std::forward<F>( f ) ) ) return;
		}

		m_queue[i % m_count].push( std::forward<F>( f ) );
	}
};	// class task_system 

/**************************************************************************************************/

task_system _system;

/**************************************************************************************************/

// template <typename>
// struct result_of_;
// 
// template <typename R, typename... Args>
// struct result_of_<R( Args... )> { using type = R; };
// 
// template <typename F>
// using result_of_t_ = typename result_of_<F>::type;

/**************************************************************************************************/

template<typename R>
struct shared_base {
	std::vector<R> m_r; // optional
	std::mutex m_mutex;
	std::condition_variable m_ready;
	std::vector<std::function<void( )>> m_then;

	virtual ~shared_base( ) { }

	void set( R&& r ) {
		std::vector<std::function<void( )>> then;
		{
			lock_t lock { m_mutex };
			m_r.push_back( std::move( r ) );
			swap( m_then, then );
		}
		m_ready.notify_all( );
		for( const auto& f : then ) _system.async_( std::move( f ) );
	}

	template <typename F>
	void then( F&& f ) {
		bool resolved { false };
		{
			lock_t lock { m_mutex };
			if( m_r.empty( ) ) {
				m_then.push_back( std::forward<F>( f ) );
			} else {
				resolved = true;
			}
		}
		if( resolved ) {
			_system.async_( std::move( f ) );
		}
	}

	const R& get( ) {
		lock_t lock { m_mutex };
		while( m_r.empty( ) ) m_ready.wait( lock );
		return m_r.back( );
	}
};

template <typename> struct shared; // not defined

template <typename R, typename... Args>
struct shared<R( Args... )>: shared_base<R> {
	std::function<R( Args... )> m_f;

	template<typename F>
	shared( F&& f ): m_f( std::forward<F>( f ) ) { }

	template <typename... A>
	void operator()( A&&... args ) {
		this->set( m_f( std::forward<A>( args )... ) );
		m_f = nullptr;
	}
};

template <typename> class packaged_task; //not defined
template <typename> class future;

template <typename S, typename F>
auto package( F&& f )->std::pair<packaged_task<S>, future<std::result_of_t<S>>>;

template <typename R>
class future {
	std::shared_ptr<shared_base<R>> m_p;

	template <typename S, typename F>
	friend auto package( F&& f )->std::pair<packaged_task<S>, future<std::result_of_t<S>>>;

	explicit future( std::shared_ptr<shared_base<R>> p ): m_p( std::move( p ) ) { }
public:
	future( ) = default;

	template <typename F>
	auto then( F&& f ) {
		auto pack = package<std::result_of_t<F( R )>( )>( [p = m_p, f = std::forward<F>( f )]( ){
			return f( p->m_r.back( ) );
		} );
		m_p->then( std::move( pack.first ) );
		return pack.second;
	}

	const R& get( ) const { return m_p->get( ); }
};

template<typename R, typename ...Args >
class packaged_task<R( Args... )> {
	std::weak_ptr<shared<R( Args... )>> _p;

	template <typename S, typename F>
	friend auto package( F&& f )->std::pair<packaged_task<S>, future<std::result_of_t<S>>>;

	explicit packaged_task( std::weak_ptr<shared<R( Args... )>> p ): _p( std::move( p ) ) { }

public:
	packaged_task( ) = default;

	template <typename... A>
	void operator()( A&&... args ) const {
		auto p = _p.lock( );
		if( p ) (*p)(std::forward<A>( args )...);
	}
};

template <typename S, typename F>
auto package( F&& f ) -> std::pair<packaged_task<S>, future<std::result_of_t<S>>> {
	auto p = make_shared<shared<S>>( std::forward<F>( f ) );
	return make_pair( packaged_task<S>( p ), future<result_of_t_<S>>( p ) );
}

template <typename F, typename ...Args>
auto async( F&& f, Args&&... args ) {
	using result_type = std::result_of_t<F( Args... )>;
	using packaged_type = packaged_task<result_type( )>;

	auto pack = package<result_type( )>( bind( std::forward<F>( f ), std::forward<Args>( args )... ) );

	_system.async_( std::move( std::get<0>( pack ) ) );
	return std::get<1>( pack );
}


/**************************************************************************************************/
