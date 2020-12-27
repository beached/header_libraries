// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <atomic>
#include <ciso646>
#include <cstddef>
#include <new>

namespace daw {
	template<typename T>
	class [[nodiscard]] atomic_unique_ptr {
		std::atomic<T *> m_ptr = static_cast<T *>( nullptr );

	public:
		atomic_unique_ptr( ) noexcept = default;

		atomic_unique_ptr( atomic_unique_ptr &&other ) noexcept
		  : m_ptr( other.m_ptr.exchange( static_cast<T *>( nullptr ),
		                                 std::memory_order_acquire ) ) {}

		atomic_unique_ptr &operator=( atomic_unique_ptr &&rhs ) noexcept {
			swap( rhs );
			return *this;
		}

		~atomic_unique_ptr( ) noexcept {
			delete m_ptr.exchange( static_cast<T *>( nullptr ),
			                       std::memory_order_acquire );
		}

		atomic_unique_ptr( atomic_unique_ptr const & ) = delete;
		atomic_unique_ptr &operator=( atomic_unique_ptr const & ) = delete;

		template<typename U>
		explicit atomic_unique_ptr( U *ptr ) noexcept
		  : m_ptr( ptr ) {}

		template<typename U>
		atomic_unique_ptr &operator=( U *ptr ) noexcept {
			delete m_ptr.exchange( static_cast<T *>( nullptr ),
			                       std::memory_order_acquire );
			m_ptr.store( ptr, std::memory_order_release );
			return *this;
		}

		template<typename U>
		explicit atomic_unique_ptr( std::atomic<U *> ptr ) noexcept
		  : m_ptr( ptr ) {}

		template<typename U>
		atomic_unique_ptr &operator=( std::atomic<U *> ptr ) noexcept {
			delete m_ptr.exchange( static_cast<T *>( nullptr ),
			                       std::memory_order_acquire );
			m_ptr.store( ptr, std::memory_order_release );
			return *this;
		}

		atomic_unique_ptr( std::nullptr_t ) noexcept
		  : m_ptr( static_cast<T *>( nullptr ) ) {}

		atomic_unique_ptr &operator=( std::nullptr_t ) noexcept {
			delete m_ptr.exchange( static_cast<T *>( nullptr ),
			                       std::memory_order_acquire );
			return *this;
		}

		[[nodiscard]] T *get( ) const noexcept {
#ifdef __cpp_lib_launder
			return std::launder( m_ptr.load( std::memory_order_acquire ) );
#else
			return m_ptr.load( std::memory_order_acquire );
#endif
		}

		[[nodiscard]] T *operator->( ) const noexcept {
			return get( );
		}

		[[nodiscard]] decltype( auto ) operator*( ) const noexcept {
			return *get( );
		}

		[[nodiscard]] explicit operator bool( ) const noexcept {
			return static_cast<bool>( get( ) );
		}

		[[nodiscard]] T *release( ) noexcept {
#ifdef __cpp_lib_launder
			return std::launder( m_ptr.exchange( static_cast<T *>( nullptr ),
			                                     std::memory_order_acquire ) );
#else
			return m_ptr.exchange( static_cast<T *>( nullptr ),
			                       std::memory_order_acquire );
#endif
		}

		void reset( ) noexcept {
			delete m_ptr.exchange( static_cast<T *>( nullptr ),
			                       std::memory_order_acquire );
		}

		template<typename U>
		void swap( atomic_unique_ptr<U> &other ) noexcept {
			// TODO: verify this is correct
			auto tmp = other.m_ptr.load( std::memory_order_acquire );
			other.m_ptr.store( m_ptr.load( std::memory_order_acquire ) );
			m_ptr.store( tmp, std::memory_order_release );
		}
	};

	template<typename T, typename... Args>
	[[nodiscard]] atomic_unique_ptr<T> make_atomic_unique_ptr( Args &&...args ) {
		if constexpr( std::is_aggregate_v<T> ) {
			return atomic_unique_ptr<T>( new T{ std::forward<Args>( args )... } );
		} else {
			return atomic_unique_ptr<T>( new T( std::forward<Args>( args )... ) );
		}
	}
} // namespace daw
