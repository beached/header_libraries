// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_assume.h"
#include "daw/daw_attributes.h"
#include "daw/daw_check_exceptions.h"
#include "daw/daw_consteval.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_function_ref.h"

#include <cmath>
#include <cstdlib>
#include <memory>
#include <optional>
#include <utility>

namespace daw {
#if defined( DAW_USE_EXCEPTIONS )
	struct daw_contract_violation : std::exception {};
	[[noreturn]] inline void default_contract_failure( ) {
		throw daw_contract_violation{ };
	}
#else
	[[noreturn]] inline void default_contract_failure( ) {
		std::abort( );
	}
#endif
	constinit static thread_local daw::function_ref<void( )>
	  contract_failure_handler = default_contract_failure;

	DAW_ATTRIB_NOINLINE inline void contract_failure( ) {
		contract_failure_handler( );
	}

	template<typename T, typename... Preconditions>
	requires( std::is_invocable_r_v<bool, Preconditions, T> and... ) //
	  class contract {
		T value;

		static constexpr bool validate( T value ) {
			return ( Preconditions{ }( value ) and ... );
		}

	public:
		DAW_ATTRIB_FLATINLINE constexpr contract( T v )
		  : value( std::move( v ) ) {
			if( not validate( value ) ) {
				contract_failure( );
			}
		}

		DAW_ATTRIB_FLATINLINE constexpr T extract( ) {
			DAW_ASSUME( validate( value ) );
			return std::move( value );
		}

		DAW_ATTRIB_FLATINLINE constexpr T &operator*( ) {
			DAW_ASSUME( validate( value ) );
			return value;
		}

		DAW_ATTRIB_FLATINLINE constexpr T const &operator*( ) const {
			DAW_ASSUME( validate( value ) );
			return value;
		}

		DAW_ATTRIB_FLATINLINE constexpr operator T const &( ) const {
			DAW_ASSUME( validate( value ) );
			return value;
		}

		template<typename... Pre2s>
		using add_precondition_t = contract<T, Preconditions..., Pre2s...>;

		template<typename... Ps>
		DAW_ATTRIB_FLATINLINE constexpr operator contract<T, Ps...>( ) const {
			DAW_ASSUME( validate( value ) );
			return contract<T, Ps...>( value );
		}

		template<typename... Pre2s>
		requires( std::is_invocable_r_v<bool, Pre2s, T> and... ) //
		  constexpr auto add_condition( Pre2s... ) const {

			DAW_ASSUME( validate( value ) );
			return contract<T, Preconditions..., Pre2s...>{ value };
		}
	};
} // namespace daw
