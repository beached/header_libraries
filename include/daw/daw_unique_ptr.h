// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_20.h"
#include "daw_cpp_feature_check.h"
#include "daw_exchange.h"
#include "daw_move.h"

#include <cstddef>
#include <functional>
#include <type_traits>

namespace daw {
	namespace unique_ptr_del {
		template<typename T>
		struct default_deleter {
			using pointer = T *;
			explicit default_deleter( ) = default;
			DAW_CPP20_CX_ALLOC void operator( )( pointer p ) const
			  noexcept( std::is_nothrow_destructible_v<T> ) {
				delete p;
			}
		};

		template<typename T>
		struct default_deleter<T[]> {
			using pointer = T *;
			explicit default_deleter( ) = default;

			DAW_CPP20_CX_ALLOC void operator( )( pointer p ) const
			  noexcept( std::is_nothrow_destructible_v<T> ) {
				delete[] p;
			}
		};

		template<typename T, typename, typename = void>
		struct pointer_type {
			using type = T *;
		};

		template<typename T, typename Deleter>
		struct pointer_type<T, Deleter, std::void_t<typename Deleter::pointer>> {
			using type = typename Deleter::pointer;
		};

		template<typename T, typename Deleter>
		using pointer_type_t = typename pointer_type<T, Deleter>::type;
	} // namespace unique_ptr_del

	template<typename T, typename Deleter = unique_ptr_del::default_deleter<T>>
	struct unique_ptr : private Deleter {
		using value_type = T;
		using pointer = unique_ptr_del::pointer_type_t<value_type, Deleter>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using deleter_t = Deleter;
		static constexpr bool is_nothrow_derefable_v =
		  noexcept( *std::declval<pointer>( ) );
		static constexpr bool is_nothrow_resetable_v =
		  std::is_nothrow_invocable_v<Deleter, pointer>;
		static_assert( not std::is_array_v<T>,
		               "It is an error to specify a bound on the array type. e.g "
		               "T[3], use T[] instead" );

	private:
		pointer m_ptr = pointer{ };

	public:
		unique_ptr( ) = default;
		constexpr unique_ptr( pointer p ) noexcept
		  : m_ptr( p ) {}

		constexpr unique_ptr( std::nullptr_t ) noexcept(
		  std::is_nothrow_default_constructible_v<pointer> )
		  : m_ptr{ } {}

		constexpr unique_ptr( unique_ptr &&other ) noexcept
		  : m_ptr( other.release( ) ) {}

		constexpr unique_ptr &
		operator=( unique_ptr &&rhs ) noexcept( is_nothrow_resetable_v ) {
			if( this != &rhs ) {
				reset( );
				m_ptr = rhs.release( );
			}
			return *this;
		}

		constexpr unique_ptr( unique_ptr const & ) = delete;
		constexpr unique_ptr &operator=( unique_ptr const & ) = delete;

		DAW_CPP20_CX_ALLOC ~unique_ptr( ) noexcept( is_nothrow_resetable_v ) {
			reset( );
		}

		constexpr pointer release( ) noexcept {
			return daw::exchange( m_ptr, pointer{ } );
		}

		constexpr deleter_t &get_deleter( ) {
			return *static_cast<Deleter *>( this );
		}

		constexpr deleter_t const &get_deleter( ) const {
			return *static_cast<Deleter const *>( this );
		}

		constexpr void reset( ) noexcept( is_nothrow_resetable_v ) {
			if( m_ptr ) {
				get_deleter( )( daw::exchange( m_ptr, pointer{ } ) );
			}
		}

		constexpr pointer get( ) const {
			return m_ptr;
		}

		constexpr pointer operator->( ) const {
			return m_ptr;
		}

		constexpr reference operator*( ) const noexcept( is_nothrow_derefable_v ) {
			return *m_ptr;
		}

		explicit constexpr operator bool( ) const noexcept {
			return static_cast<bool>( m_ptr );
		}

		constexpr void swap( unique_ptr &other ) noexcept {
			using std::swap;
			swap( m_ptr, other.m_ptr );
		}
	};

	template<typename T, typename Deleter>
	struct unique_ptr<T[], Deleter> : private Deleter {
		using value_type = T;
		using pointer = unique_ptr_del::pointer_type_t<value_type, Deleter>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using deleter_t = Deleter;
		static constexpr bool is_nothrow_derefable_v =
		  noexcept( *std::declval<pointer>( ) );
		static constexpr bool is_nothrow_resetable_v =
		  std::is_nothrow_invocable_v<Deleter, pointer>;

	private:
		pointer m_ptr = pointer{ };

	public:
		unique_ptr( ) = default;
		constexpr unique_ptr( pointer p ) noexcept
		  : m_ptr( p ) {}

		constexpr unique_ptr( std::nullptr_t ) noexcept(
		  std::is_nothrow_default_constructible_v<pointer> )
		  : m_ptr{ } {}

		constexpr unique_ptr( unique_ptr &&other ) noexcept
		  : m_ptr( other.release( ) ) {}

		constexpr unique_ptr &
		operator=( unique_ptr &&rhs ) noexcept( is_nothrow_resetable_v ) {
			if( this != &rhs ) {
				reset( );
				m_ptr = rhs.release( );
			}
			return *this;
		}

		constexpr unique_ptr( unique_ptr const & ) = delete;
		constexpr unique_ptr &operator=( unique_ptr const & ) = delete;

		DAW_CPP20_CX_ALLOC ~unique_ptr( ) noexcept( is_nothrow_resetable_v ) {
			reset( );
		}

		constexpr pointer release( ) noexcept {
			return daw::exchange( m_ptr, pointer{ } );
		}

		constexpr deleter_t &get_deleter( ) {
			return *static_cast<Deleter *>( this );
		}

		constexpr deleter_t const &get_deleter( ) const {
			return *static_cast<Deleter const *>( this );
		}

		constexpr void reset( ) noexcept( is_nothrow_resetable_v ) {
			if( m_ptr ) {
				get_deleter( )( daw::exchange( m_ptr, pointer{ } ) );
			}
		}

		constexpr pointer get( ) const {
			return m_ptr;
		}

		constexpr pointer operator->( ) const {
			return m_ptr;
		}

		constexpr reference operator*( ) const noexcept( is_nothrow_derefable_v ) {
			return *m_ptr;
		}

		explicit constexpr operator bool( ) const noexcept {
			return static_cast<bool>( m_ptr );
		}

		constexpr void swap( unique_ptr &other ) noexcept {
			using std::swap;
			swap( m_ptr, other.m_ptr );
		}

		constexpr reference operator[]( std::size_t index ) const
		  noexcept( is_nothrow_derefable_v ) {
			return m_ptr[index];
		}
	};

	template<typename T, typename... Args>
	DAW_CPP20_CX_ALLOC auto make_unique( Args &&...args )
	  -> std::enable_if_t<not std::is_array_v<T>, unique_ptr<T>> {
		if constexpr( std::is_aggregate_v<T> ) {
			return unique_ptr<T>( new T{ DAW_FWD( args )... } );
		} else {
			return unique_ptr<T>( new T( DAW_FWD( args )... ) );
		}
	}

	template<typename T>
	DAW_CPP20_CX_ALLOC auto make_unique_for_overwrite( )
	  -> std::enable_if_t<not std::is_array_v<T>, unique_ptr<T>> {
		return unique_ptr<T>( new T );
	}

	template<typename T>
	DAW_CPP20_CX_ALLOC auto make_unique( std::size_t count )
	  -> std::enable_if_t<std::is_array_v<T>, unique_ptr<T>> {
		using value_type = std::remove_extent_t<T>;
		static_assert( daw::is_unbounded_array_v<T>,
		               "It is an error to specify a bound on the array type. e.g "
		               "T[3], use T[] instead" );
		return unique_ptr<T>( new value_type[count]{ } );
	}

	template<typename T, typename... Elements>
	DAW_CPP20_CX_ALLOC auto make_unique_array( Elements &&...elements )
	  -> std::enable_if_t<std::is_array_v<T>, unique_ptr<T>> {
		static_assert( daw::is_unbounded_array_v<T>,
		               "It is an error to specify a bound on the array type. e.g "
		               "T[3], use T[] instead" );
		using value_type = std::remove_extent_t<T>;
		return unique_ptr<T>(
		  new value_type[sizeof...( Elements )]{ DAW_FWD( elements )... } );
	}

	template<typename T>
	constexpr bool operator==( unique_ptr<T> const &lhs,
	                           unique_ptr<T> const &rhs ) noexcept {
		return lhs.get( ) == rhs.get( );
	}

	template<typename T>
	constexpr bool operator!=( unique_ptr<T> const &lhs,
	                           unique_ptr<T> const &rhs ) noexcept {
		return lhs.get( ) != rhs.get( );
	}

	template<typename T>
	constexpr bool operator<( unique_ptr<T> const &lhs,
	                          unique_ptr<T> const &rhs ) noexcept {
		return std::less<void>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename T>
	constexpr bool operator<=( unique_ptr<T> const &lhs,
	                           unique_ptr<T> const &rhs ) noexcept {
		return not( lhs < rhs );
	}

	template<typename T>
	constexpr bool operator>( unique_ptr<T> const &lhs,
	                          unique_ptr<T> const &rhs ) noexcept {
		return rhs < lhs;
	}

	template<typename T>
	constexpr bool operator>=( unique_ptr<T> const &lhs,
	                           unique_ptr<T> const &rhs ) noexcept {
		return not( lhs < rhs );
	}
} // namespace daw
