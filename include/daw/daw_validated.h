// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "daw_move.h"
#include "daw_traits.h"
#include "daw_utility.h"

/// Provide a class to validate a type T after construction
/// Validator must return true if the value is valid, false otherwise
namespace daw {
	template<typename T, typename Validator>
	struct validated {
		static_assert( std::is_same_v<T, daw::remove_cvref_t<T>>,
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
				throw std::out_of_range( "Argument did not pass validation" );
			}
			return std::forward<U>( value );
		}

		template<typename U, typename... Args>
		constexpr U make_value( Args &&... args ) {
			if constexpr( std::is_enum_v<U> ) {
				return static_cast<value_t>( validate(
				  std::underlying_type_t<value_t>( std::forward<Args>( args )... ) ) );
			} else {
				return validate( daw::construct_a<U>( std::forward<Args>( args )... ) );
			}
		}

	public:
		constexpr validated( ) = default;

		template<typename U,
		         std::enable_if_t<
		           not std::is_same_v<daw::remove_cvref_t<U>, validated> and
		             not std::is_same_v<daw::remove_cvref_t<U>, std::in_place_t>,
		           std::nullptr_t> = nullptr>
		constexpr validated( U &&v )
		  : m_value( make_value<value_t>( std::forward<U>( v ) ) ) {}

		template<typename... Args>
		constexpr validated( std::in_place_t, Args &&... args )
		  : m_value( make_value<value_t>( std::forward<Args>( args )... ) ) {}

		constexpr validated &operator=( const_reference rhs ) {
			m_value = validate( rhs );
			return *this;
		}

		constexpr validated &operator=( value_t &&rhs ) {
			m_value = validate( daw::move( rhs ) );
			return *this;
		}

		constexpr const_reference get( ) const &noexcept {
			return m_value;
		}

		constexpr value_t get( ) &&noexcept {
			return daw::move( m_value );
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
