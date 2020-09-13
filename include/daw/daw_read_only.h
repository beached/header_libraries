// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#include "cpp_17.h"
#include "daw_move.h"

#pragma once

namespace daw {
	template<typename T>
	struct read_only {
		static_assert( not std::is_reference_v<T>,
		               "Reference types are not supported" );
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using rvalue_reference = value_type &&;
		using const_pointer = value_type const *;

	private:
		value_type m_value;

	public:
		constexpr read_only( ) noexcept( std::is_nothrow_constructible_v<T> )
		  : m_value( ) {}

		constexpr read_only( rvalue_reference value ) noexcept(
		  std::is_nothrow_move_constructible_v<T> )
		  : m_value( daw::move( value ) ) {}

		constexpr read_only( const_reference value ) noexcept(
		  std::is_nothrow_copy_constructible_v<T> )
		  : m_value( value ) {}

		read_only &operator=( const_reference ) = delete;
		read_only &operator=( rvalue_reference ) = delete;

		constexpr operator const_reference( ) const noexcept {
			return m_value;
		}

		constexpr const_reference get( ) const noexcept {
			return m_value;
		}

		constexpr reference read_write( ) noexcept {
			return m_value;
		}

		constexpr const_pointer operator->( ) const noexcept {
			return &m_value;
		}

		constexpr value_type move_out( ) noexcept {
			return daw::move( m_value );
		}
	}; // read_only
} // namespace daw
