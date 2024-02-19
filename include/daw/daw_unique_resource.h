// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"

#include <optional>
#include <type_traits>

namespace daw {
	template<typename T, typename Deleter>
	class unique_resource {
		std::optional<T> value;

	public:
		explicit unique_resource( ) = default;
		constexpr unique_resource( unique_resource &&other ) noexcept(
		  std::is_nothrow_move_constructible_v<T> )
		  : value( std::exchange( other.value, std::nullopt ) ) {}

		unique_resource &operator=( unique_resource &&rhs ) noexcept(
		  std::is_nothrow_move_assignable_v<T> ) {
			reset( rhs.release( ) );
			return *this;
		}

		unique_resource( unique_resource const & ) = delete;
		unique_resource &operator=( unique_resource const & ) = delete;

		~unique_resource( ) noexcept( std::is_nothrow_destructible_v<T> ) {
			reset( );
		}

		explicit unique_resource( T v ) noexcept(
		  std::is_nothrow_move_constructible_v<T> )
		  : value( std::move( v ) ) {}

		template<typename... Args>
		constexpr T &emplace( Args &&...args ) noexcept(
		  std::is_nothrow_constructible_v<T, Args...> ) {
			reset( T{ DAW_FWD( args )... } );
			return *value;
		}

		constexpr void reset( ) noexcept( std::is_nothrow_destructible_v<T> ) {
			if( value ) {
				Deleter{ }( *value );
			}
		}

		constexpr void
		reset( T new_value ) noexcept( std::is_nothrow_destructible_v<T> and
		                                 std::is_nothrow_move_constructible_v<T> ) {
			if( value ) {
				Deleter{ }( *value );
			}
			value = std::move( new_value );
		}

		constexpr T release( ) noexcept( std::is_nothrow_move_constructible_v<T> ) {
			auto result = std::exchange( value, std::nullopt );
			return std::move( *result );
		}

		constexpr T &get( ) noexcept {
			assert( value );
			return *value;
		}

		constexpr T const &get( ) const noexcept {
			assert( value );
			return *value;
		}

		constexpr T &operator*( ) noexcept {
			return get( );
		}

		constexpr T const &operator*( ) const noexcept {
			return get( );
		}

		constexpr T *operator->( ) noexcept {
			assert( value );
			return &*value;
		}

		constexpr T const *operator->( ) const noexcept {
			assert( value );
			return &*value;
		}
	};
} // namespace daw
