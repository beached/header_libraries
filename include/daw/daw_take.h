// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_concepts.h"
#include "daw_move.h"

namespace daw {
	template<typename T>
	inline constexpr bool reset_on_take = true;

	template<default_constructible T>
	constexpr T
	take( T &value ) noexcept( std::is_nothrow_move_constructible_v<T> and
	                             std::is_nothrow_default_constructible_v<T> ) {
		auto result = DAW_MOVE( value );
		if constexpr( reset_on_take<T> ) {
			value = T{ };
		}
		return result;
	}

	template<typename T>
	struct take_t {
		constexpr T operator( )( T &value ) const
		  noexcept( noexcept( take( value ) ) ) {
			return take( value );
		}
	};

	template<typename T, typename OnMove = take_t<T>>
	struct move_to_t {
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		T value;

	public:
		move_to_t( ) = default;
		~move_to_t( ) = default;
		move_to_t( move_to_t const & ) = default;
		move_to_t &operator=( move_to_t const & ) = default;

		move_to_t( move_to_t &&other ) noexcept(
		  std::is_nothrow_invocable_v<OnMove, T &> )
		  : value( OnMove{ }( other.value ) ) {}

		move_to_t &operator=( move_to_t &&rhs ) noexcept(
		  std::is_nothrow_invocable_v<OnMove, T &> ) {
			if( this != &rhs ) {
				value = OnMove{ }( rhs.value );
			}
			return *this;
		}

		template<typename Arg, typename... Args>
		  requires( not same_as<std::remove_cvref_t<Arg>, move_to_t> )
		constexpr move_to_t( Arg &&arg, Args &&...args ) noexcept(
		  std::is_nothrow_constructible_v<T, Arg, Args...> )
		  : value{ DAW_FWD( arg ), DAW_FWD( args )... } {}

		constexpr reference operator*( ) &noexcept {
			return value;
		}

		constexpr const_reference operator*( ) const &noexcept {
			return value;
		}

		constexpr value_type operator*( ) &&noexcept {
			return OnMove( value );
		}

		constexpr const_reference operator*( ) const &&noexcept {
			return value;
		}

		constexpr pointer operator->( ) noexcept {
			return &value;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return &value;
		}

		constexpr reference get( ) &noexcept {
			return value;
		}

		constexpr const_reference get( ) const &noexcept {
			return value;
		}

		constexpr value_type get( ) &&noexcept {
			return OnMove( value );
		}

		constexpr const_reference get( ) const &&noexcept {
			return value;
		}

		constexpr auto operator<=>( move_to_t const & ) const = default;
		constexpr bool operator==( move_to_t const & ) const = default;
	};

} // namespace daw
