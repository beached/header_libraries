// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_move.h"
#include "daw/daw_unique_ptr.h"

namespace daw {
	template<typename T, typename Deleter = daw::default_deleter<T>>
	struct maybe_unique_ptr : private Deleter {
		using value_type = T;
		using pointer = unique_ptr_del::pointer_type_t<value_type, Deleter>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using deleter_t = Deleter;

	private:
		daw::unique_ptr<T, Deleter> m_ptr = nullptr;
		bool m_owned = true;

		template<typename, typename>
		friend struct maybe_unique_ptr;

	public:
		maybe_unique_ptr( ) = default;

		explicit constexpr maybe_unique_ptr( pointer ptr ) noexcept
		  : m_ptr( ptr ) {}

		template<typename B>
		requires( not std::is_pointer_v<B> ) //
		  explicit constexpr maybe_unique_ptr( pointer ptr, B owned ) noexcept
		  : m_ptr( ptr )
		  , m_owned( owned ) {}

		template<explicitly_convertible_to<Deleter> D>
		explicit constexpr maybe_unique_ptr( pointer ptr, D &&deleter ) noexcept
		  : m_ptr( ptr, DAW_FWD( deleter ) ) {}

		template<explicitly_convertible_to<Deleter> D, typename B>
		requires( not std::is_pointer_v<B> ) //
		  explicit constexpr maybe_unique_ptr( pointer ptr, D &&deleter,
		                                       B owned ) noexcept
		  : m_ptr( ptr, DAW_FWD( deleter ) )
		  , m_owned( owned ) {}

		template<typename U>
		requires( not std::is_same_v<T, U> ) //
		  constexpr maybe_unique_ptr( U *p ) noexcept
		  : m_ptr( p ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		template<typename U, typename B>
		requires( not std::is_same_v<T, U> and not std::is_pointer_v<B> ) //
		  constexpr maybe_unique_ptr( U *p, B owned ) noexcept
		  : m_ptr( p )
		  , m_owned( owned ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		constexpr maybe_unique_ptr( std::nullptr_t ) noexcept {}

		constexpr maybe_unique_ptr( maybe_unique_ptr &&other ) noexcept
		  : m_ptr( other.release( ) )
		  , m_owned( std::exchange( other.m_owned, true ) ) {}

		template<typename U, typename D>
		requires( not std::is_same_v<T, U> ) //
		  constexpr maybe_unique_ptr( maybe_unique_ptr<U, D> &&other ) noexcept
		  : Deleter( static_cast<D &&>( other ) )
		  , m_ptr( other.release( ) )
		  , m_owned( std::exchange( other.m_owned, true ) ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		template<typename U, typename D>
		requires( not std::is_same_v<T, U> ) //
		  constexpr maybe_unique_ptr( unique_ptr<U, D> &&other ) noexcept
		  : Deleter( std::move( other ).get_deleter( ) )
		  , m_ptr( other.release( ) ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		constexpr maybe_unique_ptr &operator=( maybe_unique_ptr &&rhs ) noexcept {
			if( this != &rhs ) {
				reset( );
				m_ptr = rhs.release( );
				m_owned = std::exchange( rhs.m_owned, true );
			}
			return *this;
		}

		template<typename U, typename D>
		requires( not std::is_same_v<T, U> ) //
		  constexpr maybe_unique_ptr &
		  operator=( maybe_unique_ptr<U, D> &&rhs ) noexcept {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
			m_ptr = std::move( rhs.m_ptr );
			m_owned = std::exchange( rhs.m_owned, true );
			return *this;
		}

		template<typename U, typename D>
		requires( not std::is_same_v<T, U> ) //
		  constexpr maybe_unique_ptr &operator=( unique_ptr<U, D> &&rhs ) noexcept {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
			m_ptr = std::move( rhs );
			m_owned = std::exchange( rhs.m_owned, true );
			return *this;
		}

		constexpr maybe_unique_ptr( maybe_unique_ptr const & ) = delete;
		constexpr maybe_unique_ptr &operator=( maybe_unique_ptr const & ) = delete;

		DAW_CPP20_CX_ALLOC ~maybe_unique_ptr( ) noexcept {
			reset( );
		}

		constexpr pointer release( ) noexcept {
			m_owned = true;
			return m_ptr.release( );
		}

		constexpr deleter_t &get_deleter( ) & {
			return m_ptr.get_deleter( );
		}

		constexpr deleter_t const &get_deleter( ) const & {
			return m_ptr.get_deleter( );
		}

		constexpr deleter_t &&get_deleter( ) && {
			return std::move( m_ptr.get_deleter( ) );
		}

		constexpr deleter_t const &&get_deleter( ) const && {
			return std::move( m_ptr.get_deleter( ) );
		}

		constexpr void reset( ) noexcept {
			if( m_owned ) {
				m_ptr.reset( );
			} else {
				(void)m_ptr.release( );
				m_owned = true;
			}
		}

		constexpr void reset( pointer p, bool owned = true ) noexcept {
			if( m_owned ) {
				m_ptr.reset( std::move( p ) );
			} else {
				(void)m_ptr.release( );
			}
			m_ptr.reset( std::move( p ) );
			m_owned = owned;
		}

		constexpr pointer get( ) const {
			return m_ptr.get( );
		}

		constexpr pointer *get_out( ) noexcept {
			return m_ptr.get_out( );
		}

		constexpr pointer operator->( ) const {
			return m_ptr.get( );
		}

		constexpr reference operator*( ) const noexcept {
			return *m_ptr;
		}

		explicit constexpr operator bool( ) const noexcept {
			return static_cast<bool>( m_ptr );
		}

		constexpr void swap( maybe_unique_ptr &other ) noexcept {
			using std::swap;
			swap( m_ptr, other.m_ptr );
			swap( m_owned, other.m_owned );
		}

		constexpr decltype( auto ) operator[]( std::size_t index ) const noexcept
		  requires( requires { m_ptr[std::size_t{ }]; } ) {
			return m_ptr[index];
		}

		template<typename F>
		constexpr auto and_then( F &&func ) && {
			return monadic_ptr::and_then( std::move( *this ), DAW_FWD( func ) );
		}

		template<typename F>
		constexpr auto or_else( F &&func ) && {
			return monadic_ptr::or_else( std::move( *this ), DAW_FWD( func ) );
		}

		template<typename F>
		constexpr auto transform( F &&func ) & {
			return monadic_ptr::transform( *this, DAW_FWD( func ) );
		}

		template<typename F>
		constexpr auto transform( F &&func ) const & {
			return monadic_ptr::transform( *this, DAW_FWD( func ) );
		}

		template<typename F>
		constexpr auto transform( F &&func ) && {
			return monadic_ptr::transform( std::move( *this ), DAW_FWD( func ) );
		}

		template<typename F>
		constexpr auto transform( F &&func ) const && {
			return monadic_ptr::transform( std::move( *this ), DAW_FWD( func ) );
		}
	};

	template<typename T, typename... Args>
	DAW_CPP20_CX_ALLOC auto make_maybe_unique( Args &&...args )
	  -> std::enable_if_t<not std::is_array_v<T>, maybe_unique_ptr<T>> {
		if constexpr( std::is_aggregate_v<T> ) {
			return maybe_unique_ptr<T>( new T{ DAW_FWD( args )... } );
		} else {
			return maybe_unique_ptr<T>( new T( DAW_FWD( args )... ) );
		}
	}

	template<typename T>
	DAW_CPP20_CX_ALLOC auto make_maybe_unique_for_overwrite( )
	  -> std::enable_if_t<not std::is_array_v<T>, maybe_unique_ptr<T>> {
		return maybe_unique_ptr<T>( new T );
	}

	template<typename T>
	DAW_CPP20_CX_ALLOC auto make_maybe_unique( std::size_t count )
	  -> std::enable_if_t<std::is_array_v<T>, maybe_unique_ptr<T>> {
		using value_type = std::remove_extent_t<T>;
		static_assert( daw::is_unbounded_array_v<T>,
		               "It is an error to specify a bound on the array type. e.g "
		               "T[3], use T[] instead" );
		return maybe_unique_ptr<T>( new value_type[count]{ } );
	}

	template<typename T, typename... Elements>
	DAW_CPP20_CX_ALLOC auto make_maybe_unique_array( Elements &&...elements )
	  -> std::enable_if_t<std::is_array_v<T>, maybe_unique_ptr<T>> {
		static_assert( daw::is_unbounded_array_v<T>,
		               "It is an error to specify a bound on the array type. e.g "
		               "T[3], use T[] instead" );
		using value_type = std::remove_extent_t<T>;
		return maybe_unique_ptr<T>(
		  new value_type[sizeof...( Elements )]{ DAW_FWD( elements )... } );
	}

	template<typename T, typename D0, typename D1 = D0>
	constexpr bool operator==( maybe_unique_ptr<T, D0> const &lhs,
	                           maybe_unique_ptr<T, D1> const &rhs ) noexcept {
		return lhs.get( ) == rhs.get( );
	}

	template<typename T, typename D0, typename D1 = D0>
	constexpr bool operator!=( maybe_unique_ptr<T, D0> const &lhs,
	                           maybe_unique_ptr<T, D1> const &rhs ) noexcept {
		return lhs.get( ) != rhs.get( );
	}

	template<typename T, typename D0, typename D1 = D0>
	constexpr bool operator<( maybe_unique_ptr<T, D0> const &lhs,
	                          maybe_unique_ptr<T, D1> const &rhs ) noexcept {
		return std::less<void>{ }( lhs.get( ), rhs.get( ) );
	}

	template<typename T, typename D0, typename D1 = D0>
	constexpr bool operator<=( maybe_unique_ptr<T, D0> const &lhs,
	                           maybe_unique_ptr<T, D1> const &rhs ) noexcept {
		return not( lhs < rhs );
	}

	template<typename T, typename D0, typename D1 = D0>
	constexpr bool operator>( maybe_unique_ptr<T, D0> const &lhs,
	                          maybe_unique_ptr<T, D1> const &rhs ) noexcept {
		return rhs < lhs;
	}

	template<typename T, typename D0, typename D1 = D0>
	constexpr bool operator>=( maybe_unique_ptr<T, D0> const &lhs,
	                           maybe_unique_ptr<T, D1> const &rhs ) noexcept {
		return not( lhs < rhs );
	}
} // namespace daw
