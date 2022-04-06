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
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using pointer = value_type *;
		using const_pointer = value_type const *;

	private:
		T value;

	public:
		take_t( ) = default;
		~take_t( ) = default;
		take_t( take_t const & ) = default;
		take_t &operator=( take_t const & ) = default;

		take_t( take_t &&other ) noexcept( noexcept( take( other.value ) ) )
		  : value( take( other.value ) ) {}

		take_t &
		operator=( take_t &&rhs ) noexcept( noexcept( take( rhs.value ) ) ) {
			if( this != &rhs ) {
				value = take( rhs.value );
			}
			return *this;
		}

		template<typename Arg, typename... Args>
		  requires( not same_as<std::remove_cvref_t<Arg>, take_t> )
		constexpr take_t( Arg &&arg, Args &&...args ) noexcept(
		  std::is_nothrow_constructible_v<T, Arg, Args...> )
		  : value{ DAW_FWD( arg ), DAW_FWD( args )... } {}

		constexpr reference operator*( ) &noexcept {
			return value;
		}

		constexpr const_reference operator*( ) const &noexcept {
			return value;
		}

		constexpr value_type operator*( ) &&noexcept {
			return take( value );
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
			return take( value );
		}

		constexpr const_reference get( ) const &&noexcept {
			return value;
		}

		constexpr auto operator<=>( take_t const & ) const = default;
		constexpr bool operator==( take_t const & ) const = default;
	};

} // namespace daw
