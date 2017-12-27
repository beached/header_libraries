// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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

#include <atomic>
#include <functional>
#include <mutex>

#include "daw_expected.h"
#include "daw_semaphore.h"
#include "daw_traits.h"

namespace daw {
	template<typename T>
	class observable_ptr;

	namespace impl {
		template<typename T>
		class locked_ptr {
			T *m_ptr;
			std::function<void( )> m_on_exit;

		public:
			template<typename OnExit>
			constexpr locked_ptr( T *ptr, OnExit on_exit ) noexcept
			  : m_ptr{ptr}
			  , m_on_exit( std::move( on_exit ) ) {}

			~locked_ptr( ) noexcept {
				m_ptr = nullptr;
				m_on_exit( );
			}

			T *operator->( ) {
				return m_ptr;
			}

			T const *operator->( ) const {
				return m_ptr;
			}

			T &operator*( ) {
				return *m_ptr;
			}

			T const &operator*( ) const {
				return &m_ptr;
			}

			explicit operator bool( ) const {
				return m_ptr != nullptr;
			}

			locked_ptr( locked_ptr && ) = default;
			locked_ptr &operator=( locked_ptr && ) = default;

			locked_ptr( ) = delete;
			locked_ptr( locked_ptr const & ) = delete;
			locked_ptr &operator=( locked_ptr const & ) = delete;
		};

		template<typename T>
		class control_block_t {
			using observer_count_t = int16_t;

			T *m_ptr; // Pointer we are guarding
			std::atomic_bool
			  m_destruct;         // The owner has gone out of scope, as soon as there are no borrows, run values destructor
			std::mutex m_on_loan; // Pointer has been borrowed
			std::atomic<observer_count_t>
			  m_observer_count; // Number of observers alive, cannot delete control block unless this is 0

			constexpr control_block_t( T *ptr ) noexcept
			  : m_ptr{ptr}
			  , m_destruct{false}
			  , m_on_loan{}
			  , m_observer_count{0} {}

			template<typename U>
			friend class observable_ptr;

		public:
			control_block_t( control_block_t const & ) = default;
			control_block_t( control_block_t && ) = default;
			control_block_t &operator=( control_block_t const & ) = default;
			control_block_t &operator=( control_block_t && ) = default;

			~control_block_t( ) {
				auto tmp = std::exchange( m_ptr, nullptr );
				if( tmp ) {
					delete tmp;
				}
			}

			bool expired( ) const noexcept {
				return m_destruct;
			}

			void destruct_if_time( ) {
				if( m_destruct.load( ) ) {
					std::lock_guard<std::mutex> lck{m_on_loan};
					auto tmp = std::exchange( m_ptr, nullptr );
					if( tmp ) {
						delete tmp;
					}
				}
			}

			locked_ptr<T> try_borrow( ) {
				if( !m_on_loan.try_lock( ) ) {
					return locked_ptr<T>( nullptr, []( ) {} );
				}
				return locked_ptr<T>{m_ptr, [&]( ) {
					                     destruct_if_time( );
					                     m_on_loan.unlock( );
				                     }};
			}

			locked_ptr<T> borrow( ) {
				m_on_loan.lock( );
				return locked_ptr<T>{m_ptr, [&]( ) {
					                     destruct_if_time( );
					                     m_on_loan.unlock( );
				                     }};
			}

			bool add_observer( ) noexcept {
				++m_observer_count;
				if( m_destruct.load( ) ) {
					--m_observer_count;
					return false;
				}
				return true;
			}

		private:
			bool remove_observer( ) noexcept {
				if( --m_observer_count <= 0 && m_destruct.load( ) ) {
					std::lock_guard<std::mutex> lck{m_on_loan};
					auto tmp = std::exchange( m_ptr, nullptr );
					if( tmp ) {
						delete tmp;
					}
				}
				return false;
			}

			bool remove_owner( ) noexcept {
				m_destruct = true;
				destruct_if_time( );
				return m_observer_count.load( ) <= 0;
			}

		public:
			static void remove_observer( control_block_t *cb ) noexcept {
				if( cb && cb->remove_observer( ) ) {
					delete cb;
				}
			}

			static void remove_owner( control_block_t *cb ) noexcept {
				if( cb->remove_owner( ) ) {
					delete cb;
				}
			}
		};
	} // namespace impl

	template<typename T>
	class observer_ptr {
		impl::control_block_t<T> *m_control_block;

	public:
		observer_ptr( ) = delete;

		/// @brief An observer of an observable_ptr
		/// @param cb Control block for observable pointer.  Must never be null, will abort if so
		constexpr observer_ptr( impl::control_block_t<T> *cb ) noexcept
		  : m_control_block{cb} {}

		void reset( ) noexcept {
			auto tmp = std::exchange( m_control_block, nullptr );
			if( tmp != nullptr ) {
				impl::control_block_t<T>::remove_observer( tmp );
			}
		}

		~observer_ptr( ) noexcept {
			reset( );
		}

		constexpr observer_ptr( observer_ptr const &other ) noexcept
		  : m_control_block{other.m_control_block} {}

		constexpr observer_ptr &operator=( observer_ptr const &rhs ) noexcept {
			if( this != &rhs ) {
				m_control_block = rhs.m_control_block;
			}
			return *this;
		}

		constexpr observer_ptr( observer_ptr &&other ) noexcept
		  : m_control_block{std::exchange( other.m_control_block, nullptr )} {}

		constexpr observer_ptr &operator=( observer_ptr &&rhs ) noexcept {
			if( this != &rhs ) {
				m_control_block = std::exchange( rhs.m_control_block, nullptr );
			}
			return *this;
		}

		impl::locked_ptr<T const> try_borrow( ) const {
			if( !m_control_block ) {
				return impl::locked_ptr<T>{nullptr, []( ) {}};
			}
			return m_control_block->try_borrow( );
		}

		impl::locked_ptr<T> try_borrow( ) {
			if( !m_control_block ) {
				return impl::locked_ptr<T>{nullptr, []( ) {}};
			}
			return m_control_block->try_borrow( );
		}

		impl::locked_ptr<T const> borrow( ) const {
			if( !m_control_block ) {
				return impl::locked_ptr<T>{nullptr, []( ) {}};
			}
			return m_control_block->borrow( );
		}

		impl::locked_ptr<T> borrow( ) {
			if( !m_control_block ) {
				return impl::locked_ptr<T>{nullptr, []( ) {}};
			}
			return m_control_block->borrow( );
		}

		template<typename Callable>
		decltype( auto ) lock( Callable c ) const noexcept( noexcept( c( std::declval<T const &>( ) ) ) ) {
			using result_t = std::decay_t<decltype( c( std::declval<T const &>( ) ) )>;
			auto lck_ptr = borrow( );
			if( !lck_ptr ) {
				return daw::expected_t<result_t>{};
			}
			T const &r = *lck_ptr;
			return daw::expected_t<result_t>::from_code( c, r );
		}

		template<typename Callable>
		decltype( auto ) lock( Callable c ) noexcept( noexcept( c( std::declval<T &>( ) ) ) ) {
			using result_t = std::decay_t<decltype( std::declval<Callable>( )( std::declval<T &>( ) ) )>;
			auto lck_ptr = borrow( );
			if( !lck_ptr ) {
				return daw::expected_t<result_t>{};
			}
			T &r = *lck_ptr;
			return daw::expected_t<result_t>::from_code( c, r );
		}

		bool expired( ) const {
			return m_control_block && m_control_block->expired( );
		}

		explicit operator bool( ) const {
			return !expired( );
		}
	};

	/// @brief A pointer wrapper that allows others to temporarily postpone destruction while in a locked scope
	/// @tparam T Type to construct/hold
	template<typename T>
	class observable_ptr {
		impl::control_block_t<T> *m_control_block;
		static constexpr intmax_t owner_id = -3;

	public:
		observable_ptr( observable_ptr const & ) = delete;
		observable_ptr &operator=( observable_ptr const & ) = delete;

		observable_ptr( observable_ptr && ) noexcept = default;
		observable_ptr &operator=( observable_ptr && ) noexcept = default;

		~observable_ptr( ) noexcept {
			auto tmp = std::exchange( m_control_block, nullptr );
			if( tmp ) {
				impl::control_block_t<T>::remove_owner( tmp );
			}
		}

		/// @brief Take ownership of pointer and construct shared_ptr with it
		/// @param value pointer to take ownershiip of
		explicit observable_ptr( T *value )
		  : m_control_block{new impl::control_block_t<T>{value}} {}

		observable_ptr( )
		  : observable_ptr{nullptr} {}

		observer_ptr<T> get_observer( ) {
			return observer_ptr<T>{m_control_block};
		}

		observer_ptr<T> get_observer( ) const {
			return observer_ptr<T>{m_control_block};
		}

		const T &operator*( ) const noexcept {
			return *m_control_block->m_ptr;
		}

		T &operator*( ) noexcept {
			return *m_control_block->m_ptr;
		}

		T const *operator->( ) const noexcept {
			return m_control_block->m_ptr;
		}

		T *operator->( ) noexcept {
			return m_control_block->m_ptr;
		}

		impl::locked_ptr<T const> try_borrow( ) const {
			observer_ptr<T> const tmp{m_control_block};
			return tmp.try_borrow( );
		}

		impl::locked_ptr<T> try_borrow( ) {
			observer_ptr<T> tmp{m_control_block};
			return tmp.try_borrow( );
		}

		impl::locked_ptr<T const> borrow( ) const {
			observer_ptr<T> const tmp{m_control_block};
			return tmp.borrow( );
		}

		impl::locked_ptr<T> borrow( ) {
			observer_ptr<T> tmp{m_control_block};
			return tmp.borrow( );
		}
		template<typename Callable>
		decltype( auto ) lock( Callable c ) const {
			observer_ptr<T> const tmp{m_control_block};
			return tmp.lock( std::move( c ) );
		}

		template<typename Callable>
		decltype( auto ) lock( Callable c ) {
			observer_ptr<T> tmp{m_control_block};
			return tmp.lock( std::move( c ) );
		}

		explicit operator bool( ) const noexcept {
			return m_control_block != nullptr && !m_control_block->expired( );
		}

		operator observer_ptr<T>( ) const noexcept {
			return observer_ptr<T>{m_control_block};
		}
	};

	template<typename T, typename... Args>
	observable_ptr<T>
	make_observable_ptr( Args &&... args ) noexcept( noexcept( new T( std::forward<Args>( args )... ) ) ) {
		return observable_ptr<T>{new T( std::forward<Args>( args )... )};
	}
} // namespace daw

