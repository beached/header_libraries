// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_20.h"
#include "daw_consteval.h"
#include "daw_exchange.h"
#include "daw_move.h"

#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	struct default_pointer_deleter {
		constexpr void operator( )( T *ptr ) const
		  noexcept( std::is_nothrow_destructible_v<T> ) {
			delete ptr;
		}
	};

	template<typename T>
	struct default_pointer_deleter<T[]> {
		constexpr void operator( )( T *ptr ) const
		  noexcept( std::is_nothrow_destructible_v<T> ) {
			delete[] ptr;
		}
	};

	namespace rc_ptr_impl {
		template<typename T, typename, typename = void>
		struct pointer_type_impl {
			using type = T *;
		};

		template<typename T, typename Deleter>
		struct pointer_type_impl<
		  T, Deleter,
		  std::void_t<typename std::remove_reference_t<Deleter>::pointer>> {
			using type = typename Deleter::pointer;
		};
	} // namespace rc_ptr_impl

	template<typename T, typename Deleter = default_pointer_deleter<T>>
	struct rc_ptr : Deleter {
		using pointer = typename rc_ptr_impl::pointer_type_impl<T, Deleter>::type;
		using element_type = T;
		using deleter_type = Deleter;
		using reference = T &;

		static constexpr bool is_t_nothrow_destructible =
		  std::is_nothrow_invocable_v<deleter_type, pointer>;

	private:
		pointer m_pointer = nullptr;
		std::size_t *m_ref_count = nullptr;

	public:
		[[nodiscard]] constexpr std::size_t &ref_count( ) noexcept {
			assert( m_ref_count );
			return *m_ref_count;
		}

		[[nodiscard]] constexpr std::size_t const &ref_count( ) const noexcept {
			assert( m_ref_count );
			return *m_ref_count;
		}

		rc_ptr( ) = default;

		constexpr rc_ptr( rc_ptr const &other ) noexcept
		  : m_pointer( other.m_pointer )
		  , m_ref_count( other.m_ref_count ) {
			if( m_ref_count ) {
				++ref_count( );
			}
		}

		constexpr rc_ptr &
		operator=( rc_ptr const &rhs ) noexcept( is_t_nothrow_destructible ) {
			if( this != &rhs ) {
				reset( );
				if( rhs ) {
					m_pointer = rhs.m_pointer;
					m_ref_count = rhs.m_ref_count;
					assert( m_ref_count );
					++ref_count( );
				}
			}
			return *this;
		}

		constexpr rc_ptr( rc_ptr &&other ) noexcept
		  : m_pointer( daw::exchange( other.m_pointer, nullptr ) )
		  , m_ref_count( daw::exchange( other.m_ref_count, nullptr ) ) {}

		constexpr rc_ptr &
		operator=( rc_ptr &&rhs ) noexcept( is_t_nothrow_destructible ) {
			if( this != &rhs ) {
				reset( );
				m_pointer = daw::exchange( rhs.m_pointer, nullptr );
				m_ref_count = daw::exchange( rhs.m_ref_count, nullptr );
			}
			return *this;
		}

		constexpr void swap( rc_ptr &other ) {
			m_pointer = daw::exchange( other.m_pointer, std::move( m_pointer ) );
			m_ref_count = daw::exchange( other.m_ref_count, m_ref_count );
		}

		DAW_CX_DTOR ~rc_ptr( ) noexcept( is_t_nothrow_destructible ) {
			reset( );
		}

		explicit constexpr rc_ptr( pointer ptr ) noexcept
		  : m_pointer( ptr ) {
			if( not ptr ) {
				return;
			}
			m_ref_count = new std::size_t{ 1 };
		}

		explicit DAW_CONSTEVAL rc_ptr( std::nullptr_t ) noexcept {}

		constexpr void reset( ) noexcept( is_t_nothrow_destructible ) {
			if( not m_pointer ) {
				return;
			}
			assert( m_ref_count );
			if( --ref_count( ) == 0 ) {
				auto old_ptr = daw::exchange( m_pointer, nullptr );
				auto old_ref = daw::exchange( m_ref_count, nullptr );
				delete old_ref;
				Deleter{ }( old_ptr );
			}
		}

		constexpr pointer get( ) const {
			return m_pointer;
		}

		constexpr pointer operator->( ) const {
			assert( m_pointer );
			return get( );
		}

		constexpr reference operator*( ) const noexcept {
			assert( m_pointer );
			return *get( );
		}

		constexpr deleter_type &get_deleter( ) noexcept {
			return *this;
		}

		constexpr deleter_type const &get_deleter( ) const noexcept {
			return *this;
		}

		constexpr explicit operator bool( ) const noexcept {
			return static_cast<bool>( m_pointer );
		}

		constexpr bool operator==( rc_ptr const &rhs ) const noexcept {
			return m_pointer == rhs.m_pointer;
		}

		constexpr bool operator!=( rc_ptr const &rhs ) const noexcept {
			return m_pointer != rhs.m_pointer;
		}

		constexpr bool operator<( rc_ptr const &rhs ) const noexcept {
			return std::less<>{ }( m_pointer, rhs.m_pointer );
		}

		constexpr bool operator<=( rc_ptr const &rhs ) const noexcept {
			return not( rhs < *this );
		}

		constexpr bool operator>( rc_ptr const &rhs ) const noexcept {
			return rhs < *this;
		}

		constexpr bool operator>=( rc_ptr const &rhs ) const noexcept {
			return not( *this < rhs );
		}

		constexpr bool operator==( std::nullptr_t ) const noexcept {
			return not m_pointer;
		}

		constexpr bool operator!=( std::nullptr_t ) const noexcept {
			return m_pointer;
		}

		constexpr bool operator<( std::nullptr_t ) const noexcept {
			return std::less<>{ }( m_pointer, nullptr );
		}

		constexpr bool operator<=( std::nullptr_t ) const noexcept {
			return not std::less<>{ }( nullptr, m_pointer );
		}

		constexpr bool operator>( std::nullptr_t ) const noexcept {
			return std::less<>{ }( nullptr, m_pointer );
		}

		constexpr bool operator>=( std::nullptr_t ) const noexcept {
			return not std::less<>{ }( m_pointer, nullptr );
		}
	};

	template<typename T, typename Deleter>
	struct rc_ptr<T[], Deleter> : Deleter {
		using pointer = typename rc_ptr_impl::pointer_type_impl<T, Deleter>::type;
		using element_type = T;
		using deleter_type = Deleter;
		using reference = T &;

		static constexpr bool is_t_nothrow_destructible =
		  std::is_nothrow_invocable_v<deleter_type, pointer>;

	private:
		pointer m_pointer = nullptr;
		std::size_t *m_ref_count = nullptr;

	public:
		[[nodiscard]] constexpr std::size_t &ref_count( ) noexcept {
			assert( m_ref_count );
			return *m_ref_count;
		}

		[[nodiscard]] constexpr std::size_t const &ref_count( ) const noexcept {
			assert( m_ref_count );
			return *m_ref_count;
		}

		rc_ptr( ) = default;

		constexpr rc_ptr( rc_ptr const &other ) noexcept
		  : m_pointer( other.m_pointer )
		  , m_ref_count( other.m_ref_count ) {
			if( not m_ref_count ) {
				assert( not m_pointer );
				return;
			}
			++ref_count( );
		}

		constexpr rc_ptr &
		operator=( rc_ptr const &rhs ) noexcept( is_t_nothrow_destructible ) {
			if( this != &rhs ) {
				reset( );
				m_pointer = rhs.m_pointer;
				m_ref_count = rhs.m_ref_count;
				if( m_pointer ) {
					assert( m_ref_count );
					++ref_count( );
				}
			}
			return *this;
		}

		constexpr rc_ptr( rc_ptr &&other ) noexcept
		  : m_pointer( daw::exchange( other.m_pointer, nullptr ) )
		  , m_ref_count( daw::exchange( other.m_ref_count, nullptr ) ) {}

		constexpr rc_ptr &
		operator=( rc_ptr &&rhs ) noexcept( is_t_nothrow_destructible ) {
			if( this != &rhs ) {
				reset( );
				m_pointer = daw::exchange( rhs.m_pointer, nullptr );
				m_ref_count = daw::exchange( rhs.m_ref_count, nullptr );
			}
			return *this;
		}

		constexpr void swap( rc_ptr &other ) {
			m_pointer = daw::exchange( other.m_pointer, std::move( m_pointer ) );
			m_ref_count = daw::exchange( other.m_ref_count, m_ref_count );
		}

		DAW_CX_DTOR ~rc_ptr( ) noexcept( is_t_nothrow_destructible ) {
			reset( );
		}

		explicit constexpr rc_ptr( pointer ptr ) noexcept
		  : m_pointer( ptr ) {
			if( not ptr ) {
				return;
			}
			m_ref_count = new std::size_t{ 1 };
		}

		explicit DAW_CONSTEVAL rc_ptr( std::nullptr_t ) noexcept {}

		constexpr void reset( ) noexcept( is_t_nothrow_destructible ) {
			if( not m_pointer ) {
				assert( not m_ref_count );
				return;
			}
			assert( m_ref_count );
			if( --ref_count( ) == 0 ) {
				auto *old_ptr = daw::exchange( m_pointer, nullptr );
				auto *old_ref = daw::exchange( m_ref_count, nullptr );
				delete old_ref;
				Deleter{ }( old_ptr );
			}
		}

		constexpr pointer get( ) const {
			return m_pointer;
		}

		constexpr pointer operator->( ) const {
			assert( m_pointer );
			return get( );
		}

		constexpr reference operator*( ) const noexcept {
			assert( m_pointer );
			return *get( );
		}

		constexpr deleter_type &get_deleter( ) noexcept {
			return *this;
		}

		constexpr deleter_type const &get_deleter( ) const noexcept {
			return *this;
		}

		constexpr explicit operator bool( ) const noexcept {
			return static_cast<bool>( m_pointer );
		}

		constexpr reference operator[]( std::size_t index ) const noexcept {
			assert( m_pointer and m_ref_count );
			return m_pointer[index];
		}

		constexpr bool operator==( rc_ptr const &rhs ) const noexcept {
			return m_pointer == rhs.m_pointer;
		}

		constexpr bool operator!=( rc_ptr const &rhs ) const noexcept {
			return m_pointer != rhs.m_pointer;
		}

		constexpr bool operator<( rc_ptr const &rhs ) const noexcept {
			return std::less<>{ }( m_pointer, rhs.m_pointer );
		}

		constexpr bool operator<=( rc_ptr const &rhs ) const noexcept {
			return not( rhs < *this );
		}

		constexpr bool operator>( rc_ptr const &rhs ) const noexcept {
			return rhs < *this;
		}

		constexpr bool operator>=( rc_ptr const &rhs ) const noexcept {
			return not( *this < rhs );
		}

		constexpr bool operator==( std::nullptr_t ) const noexcept {
			return not m_pointer;
		}

		constexpr bool operator!=( std::nullptr_t ) const noexcept {
			return m_pointer;
		}

		constexpr bool operator<( std::nullptr_t ) const noexcept {
			return std::less<>{ }( m_pointer, nullptr );
		}

		constexpr bool operator<=( std::nullptr_t ) const noexcept {
			return not std::less<>{ }( nullptr, m_pointer );
		}

		constexpr bool operator>( std::nullptr_t ) const noexcept {
			return std::less<>{ }( nullptr, m_pointer );
		}

		constexpr bool operator>=( std::nullptr_t ) const noexcept {
			return not std::less<>{ }( m_pointer, nullptr );
		}
	};

	template<typename T, typename D>
	constexpr void swap( rc_ptr<T, D> &lhs, rc_ptr<T, D> &rhs ) {
		lhs.swap( rhs );
	}

	namespace rc_ptr_impl {
		template<typename T>
		struct make_rc_ptr_type_impl {
			using single_object = rc_ptr<T>;
		};

		template<typename T>
		struct make_rc_ptr_type_impl<T[]> {
			using array_object = rc_ptr<T[]>;
		};

		template<typename T>
		using make_rc_ptr_single = typename make_rc_ptr_type_impl<T>::single_object;
		template<typename T>
		using make_rc_ptr_array = typename make_rc_ptr_type_impl<T>::array_object;
	} // namespace rc_ptr_impl

	template<typename T, typename... Args>
	DAW_CPP20_CX_ALLOC rc_ptr_impl::make_rc_ptr_single<T>
	make_rc_ptr( Args &&...args ) {
		static_assert( not std::is_array_v<T>, "Unexpected array type" );
		if constexpr( std::is_aggregate_v<T> ) {
			return rc_ptr<T>( new T{ DAW_FWD( args )... } );
		} else {
			return rc_ptr<T>( new T( DAW_FWD( args )... ) );
		}
	}

	template<typename T>
	DAW_CPP20_CX_ALLOC rc_ptr_impl::make_rc_ptr_array<T>
	make_rc_ptr( std::size_t size ) {
		static_assert( std::is_array_v<T>, "Expected and array type" );
		return rc_ptr<T>( new std::remove_extent_t<T>[size] {} );
	}

	template<typename T, typename... Args>
	DAW_CPP20_CX_ALLOC rc_ptr_impl::make_rc_ptr_single<T>
	make_rc_ptr_for_overwrite( Args &&... ) = delete;

	template<typename T>
	DAW_CPP20_CX_ALLOC rc_ptr_impl::make_rc_ptr_array<T>
	make_rc_ptr_for_overwrite( std::size_t size ) {
		static_assert( std::is_array_v<T>, "Expected and array type" );
		return rc_ptr<T>( new std::remove_extent_t<T>[size] );
	}
} // namespace daw
