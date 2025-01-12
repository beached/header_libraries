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
#include "daw_monadic_pointer_ops.h"
#include "daw_move.h"

#include <cstddef>
#include <functional>
#include <type_traits>

namespace daw {
	namespace unique_ptr_details {
		template<typename T, typename U>
		inline constexpr bool is_safe_child_type_v =
		  not std::is_same_v<T, U> and std::has_virtual_destructor_v<T> and
		  std::is_base_of_v<T, U>;
	}

	template<typename T>
	struct default_deleter {
		using pointer = T *;
		using does_validity_check = void;
		static constexpr pointer default_pointer_value = nullptr;

		explicit default_deleter( ) = default;

		template<typename U, std::enable_if_t<not std::is_same_v<T, U>,
		                                      std::nullptr_t> = nullptr>
		constexpr default_deleter( default_deleter<U> const & ) noexcept {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		template<typename P, std::enable_if_t<std::is_constructible_v<pointer, P *>,
		                                      std::nullptr_t> = nullptr>
		DAW_CPP20_CX_ALLOC void operator( )( P *p ) const noexcept {
			static_assert( sizeof( P ) > 0,
			               "One cannot delete a pointer to an incomplete type" );
			delete static_cast<pointer>( p );
		}
	};

	template<typename T>
	struct default_deleter<T[]> {
		using pointer = T *;
		using does_validity_check = void;
		static constexpr pointer default_pointer_value = nullptr;

		explicit default_deleter( ) = default;

		DAW_CPP20_CX_ALLOC void operator( )( pointer p ) const noexcept {
			delete[] p;
		}
	};

	namespace unique_ptr_del {
		template<typename T, typename = void>
		inline constexpr bool does_validity_check_v = false;

		template<typename T>
		inline constexpr bool
		  does_validity_check_v<T, std::void_t<typename T::does_validity_check>> =
		    true;

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

		template<typename T, typename Deleter, typename = void>
		inline constexpr auto default_pointer_value_v =
		  pointer_type_t<T, Deleter>{ };

		template<typename T, typename Deleter>
		inline constexpr auto default_pointer_value_v<
		  T, Deleter, std::void_t<decltype( Deleter::default_pointer_value )>> =
		  Deleter::default_pointer_value;
	} // namespace unique_ptr_del

	template<typename T, typename Deleter = default_deleter<T>>
	struct unique_ptr : private Deleter {
		using value_type = T;
		using pointer = unique_ptr_del::pointer_type_t<value_type, Deleter>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using deleter_t = Deleter;

		static_assert( not std::is_array_v<T>,
		               "It is an error to specify a bound on the array type. e.g "
		               "T[3], use T[] instead" );

	private:
		template<typename, typename>
		friend struct unique_ptr;

		pointer m_ptr = unique_ptr_del::default_pointer_value_v<T, Deleter>;

	public:
		unique_ptr( ) = default;
		constexpr unique_ptr( pointer p ) noexcept
		  : m_ptr( p ) {}

		template<typename U, std::enable_if_t<not std::is_same_v<T, U>,
		                                      std::nullptr_t> = nullptr>
		constexpr unique_ptr( U *p ) noexcept
		  : m_ptr( p ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		constexpr unique_ptr( std::nullptr_t ) noexcept {}

		constexpr unique_ptr( unique_ptr &&other ) noexcept
		  : m_ptr( other.release( ) ) {}

		template<
		  typename U, typename D,
		  std::enable_if_t<not std::is_same_v<T, U>, std::nullptr_t> = nullptr>
		constexpr unique_ptr( unique_ptr<U, D> &&other ) noexcept
		  : Deleter( static_cast<D &&>( other ) )
		  , m_ptr( other.release( ) ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		constexpr unique_ptr &operator=( unique_ptr &&rhs ) noexcept {
			if( this != &rhs ) {
				reset( );
				m_ptr = rhs.release( );
			}
			return *this;
		}

		template<typename U, std::enable_if_t<not std::is_same_v<T, U>,
		                                      std::nullptr_t> = nullptr>
		constexpr unique_ptr &operator=( unique_ptr<U, Deleter> &&rhs ) noexcept {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
			if( this != &rhs ) {
				reset( );
				m_ptr = rhs.release( );
			}
			return *this;
		}

		constexpr unique_ptr( unique_ptr const & ) = delete;
		constexpr unique_ptr &operator=( unique_ptr const & ) = delete;

		DAW_CPP20_CX_ALLOC ~unique_ptr( ) noexcept {
			reset( );
		}

		constexpr pointer release( ) noexcept {
			return daw::exchange(
			  m_ptr, unique_ptr_del::default_pointer_value_v<T, Deleter> );
		}

		constexpr deleter_t &get_deleter( ) {
			return *static_cast<Deleter *>( this );
		}

		constexpr deleter_t const &get_deleter( ) const {
			return *static_cast<Deleter const *>( this );
		}

		constexpr void reset( ) noexcept {
			if( unique_ptr_del::does_validity_check_v<Deleter> or m_ptr ) {
				get_deleter( )( daw::exchange(
				  m_ptr, unique_ptr_del::default_pointer_value_v<T, Deleter> ) );
			}
		}

		constexpr pointer get( ) const {
			return m_ptr;
		}

		constexpr pointer operator->( ) const {
			return m_ptr;
		}

		constexpr reference operator*( ) const noexcept {
			return *m_ptr;
		}

		explicit constexpr operator bool( ) const noexcept {
			return static_cast<bool>( m_ptr );
		}

		constexpr void swap( unique_ptr &other ) noexcept {
			using std::swap;
			swap( m_ptr, other.m_ptr );
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

	template<typename T, typename Deleter>
	struct unique_ptr<T[], Deleter> : private Deleter {
		using value_type = T;
		using pointer = unique_ptr_del::pointer_type_t<value_type, Deleter>;
		using reference = std::add_lvalue_reference_t<value_type>;
		using deleter_t = Deleter;

	private:
		pointer m_ptr = unique_ptr_del::default_pointer_value_v<T, Deleter>;

	public:
		unique_ptr( ) = default;
		constexpr unique_ptr( pointer p ) noexcept
		  : m_ptr( p ) {}

		template<typename U, std::enable_if_t<not std::is_same_v<T, U>,
		                                      std::nullptr_t> = nullptr>
		constexpr unique_ptr( U *p ) noexcept
		  : m_ptr( p ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		constexpr unique_ptr( std::nullptr_t ) noexcept {}

		constexpr unique_ptr( unique_ptr &&other ) noexcept
		  : m_ptr( other.release( ) ) {}

		template<typename U, std::enable_if_t<not std::is_same_v<T, U>,
		                                      std::nullptr_t> = nullptr>
		constexpr unique_ptr( unique_ptr<U, Deleter> &&other ) noexcept
		  : m_ptr( other.release( ) ) {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
		}

		constexpr unique_ptr &operator=( unique_ptr &&rhs ) noexcept {
			if( this != &rhs ) {
				reset( );
				m_ptr = rhs.release( );
			}
			return *this;
		}

		template<typename U, std::enable_if_t<not std::is_same_v<T, U>,
		                                      std::nullptr_t> = nullptr>
		constexpr unique_ptr &operator=( unique_ptr<U, Deleter> &&rhs ) noexcept {
			static_assert( unique_ptr_details::is_safe_child_type_v<T, U>,
			               "One cannot destruct U, if T does not have a virtual "
			               "destructor, and U being a child of T" );
			if( this != &rhs ) {
				reset( );
				m_ptr = rhs.release( );
			}
			return *this;
		}

		constexpr unique_ptr( unique_ptr const & ) = delete;
		constexpr unique_ptr &operator=( unique_ptr const & ) = delete;

		DAW_CPP20_CX_ALLOC ~unique_ptr( ) noexcept {
			reset( );
		}

		constexpr pointer release( ) noexcept {
			return daw::exchange(
			  m_ptr, unique_ptr_del::default_pointer_value_v<T, Deleter> );
		}

		constexpr deleter_t &get_deleter( ) {
			return *static_cast<Deleter *>( this );
		}

		constexpr deleter_t const &get_deleter( ) const {
			return *static_cast<Deleter const *>( this );
		}

		constexpr void reset( ) noexcept {
			if( unique_ptr_del::does_validity_check_v<Deleter> or m_ptr ) {
				get_deleter( )( daw::exchange(
				  m_ptr, unique_ptr_del::default_pointer_value_v<T, Deleter> ) );
			}
		}

		constexpr pointer get( ) const {
			return m_ptr;
		}

		constexpr pointer operator->( ) const {
			return m_ptr;
		}

		constexpr reference operator*( ) const noexcept {
			return *m_ptr;
		}

		explicit constexpr operator bool( ) const noexcept {
			return static_cast<bool>( m_ptr );
		}

		constexpr void swap( unique_ptr &other ) noexcept {
			using std::swap;
			swap( m_ptr, other.m_ptr );
		}

		constexpr reference operator[]( std::size_t index ) const noexcept {
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
