// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_enable_if.h"
#include "daw_visit.h"

#include <ciso646>
#include <type_traits>
#include <variant>

namespace daw {
	template<typename Base, typename... Types>
	struct poly_var {
		static_assert( ( std::is_base_of_v<Base, Types> and ... ),
		               "Types must be derived from base" );
		std::variant<Types...> m_value;

		constexpr poly_var( ) = default;

		template<typename T, daw::enable_when_t<std::is_constructible_v<
		                       std::variant<Types...>, T>> = nullptr>
		constexpr poly_var( T &&value )
		  : m_value( std::forward<T>( value ) ) {}

		template<typename T, daw::enable_when_t<std::is_constructible_v<
		                       std::variant<Types...>, T>> = nullptr>
		constexpr poly_var &operator=( T &&value ) {
			m_value = std::forward<T>( value );
			return *this;
		}

		constexpr Base const &operator*( ) const {
			return daw::visit_nt( m_value, []( auto const &value ) {
				return *static_cast<Base const *>( &value );
			} );
		}

		constexpr Base &operator*( ) {
			return daw::visit_nt(
			  m_value, []( auto &value ) { return *static_cast<Base *>( &value ); } );
		}

		constexpr Base const *operator->( ) const {
			return daw::visit_nt( m_value, []( auto const &value ) {
				return static_cast<Base const *>( &value );
			} );
		}

		constexpr Base *operator->( ) {
			return daw::visit_nt(
			  m_value, []( auto &value ) { return static_cast<Base *>( &value ); } );
		}
	};
} // namespace daw
