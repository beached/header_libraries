// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_construct_a.h"
#include "daw_move.h"
#include "daw_remove_cvref.h"
#include "daw_traits_is_same.h"

#include <cstddef>
#include <daw/stdinc/in_place.h>
#include <iso646.h>
#include <type_traits>

/// Provide a class to validate a type T after construction
/// Validator must return true if the value is valid, false otherwise
namespace daw {
	template<typename T, typename Validator>
	struct validated {
		static_assert( daw::is_same_v<T, daw::remove_cvref_t<T>>,
		               "T cannot be cv or ref qualified" );
		// This is really true for ref qualified T as it allows the value to change
		// without validation

		using value_t = T;
		using const_reference = value_t const &;
		using const_pointer = value_t const *;

	private:
		value_t m_value{ };

		template<typename U>
		constexpr decltype( auto ) validate( U &&value ) {
			if( not Validator{ }( value ) ) {
				DAW_THROW_OR_TERMINATE( std::out_of_range,
				                        "Argument did not pass validation" );
			}
			return DAW_FWD( value );
		}

		template<typename U, typename... Args>
		constexpr U make_value( Args &&...args ) {
			if constexpr( std::is_enum_v<U> ) {
				return static_cast<value_t>(
				  validate( std::underlying_type_t<value_t>( DAW_FWD( args )... ) ) );
			} else {
				return validate( daw::construct_a<U>( DAW_FWD( args )... ) );
			}
		}

	public:
		constexpr validated( ) = default;

		template<typename U,
		         std::enable_if_t<
		           not daw::is_same_v<daw::remove_cvref_t<U>, validated> and
		             not daw::is_same_v<daw::remove_cvref_t<U>, std::in_place_t>,
		           std::nullptr_t> = nullptr>
		constexpr validated( U &&v )
		  : m_value( make_value<value_t>( DAW_FWD( v ) ) ) {}

		template<typename... Args>
		constexpr validated( std::in_place_t, Args &&...args )
		  : m_value( make_value<value_t>( DAW_FWD( args )... ) ) {}

		constexpr validated &operator=( const_reference rhs ) {
			m_value = validate( rhs );
			return *this;
		}

		constexpr validated &operator=( value_t &&rhs ) {
			m_value = validate( DAW_MOVE( rhs ) );
			return *this;
		}

		constexpr const_reference get( ) const & noexcept {
			return m_value;
		}

		constexpr value_t get( ) && noexcept {
			return DAW_MOVE( m_value );
		}

		constexpr operator const_reference( ) const noexcept {
			return m_value;
		}

		constexpr const_reference operator*( ) const noexcept {
			return m_value;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return &m_value;
		}
	}; // namespace daw
} // namespace daw
