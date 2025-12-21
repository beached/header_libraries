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
#include "daw/daw_callable.h"
#include "daw/daw_check_exceptions.h"
#include "daw/daw_consteval.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_function_ref.h"
#include "daw/daw_move.h"

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
	DAW_CONSTINIT static thread_local daw::function_ref<void( )>
	  contract_failure_handler = default_contract_failure;

	DAW_ATTRIB_NOINLINE inline void contract_failure( ) {
		contract_failure_handler( );
	}

	template<typename T, typename... Preconditions>
	class contract {
		T value;

		static_assert( ( daw::is_callable_r_v<bool, Preconditions, T> and ... ),
		               "Preconditions must be callable with T and the return must "
		               "convert to bool" );

		static constexpr bool validate( T value ) {
			return ( Preconditions{ }( value ) and ... );
		}

	public:
#if defined( DAW_ATTRIB_ENABLE_IF )
		DAW_CONSTEVAL contract( T v )
		  DAW_ATTRIB_ENABLE_IF( __builtin_constant_p( v ) and validate( v ),
		                        "Contract violatiion" )
		  : value( DAW_FWD( v ) ) {}

		DAW_ATTRIB_FLATINLINE constexpr contract( T v )
		  DAW_ATTRIB_ENABLE_IF( not __builtin_constant_p( v ), " " )
		  : value( DAW_FWD( v ) ) {
			if( not validate( value ) ) {
				contract_failure( );
			}
		}
#if defined( DAW_HAS_CPP26_DELETED_REASON )
		DAW_CONSTEVAL contract( T v ) DAW_ATTRIB_ENABLE_IF(
		  __builtin_constant_p( v ) and not validate( v ),
		  "Contract violatiion" ) = delete( "Contract violation" );
#endif
#else
		DAW_ATTRIB_FLATINLINE constexpr contract( T v )
		  : value( DAW_FWD( v ) ) {
			if( not validate( value ) ) {
				contract_failure( );
			}
		}
#endif

		DAW_ATTRIB_FLATINLINE constexpr T extract( ) {
			DAW_ASSUME( validate( value ) );
			return DAW_FWD( value );
		}

		DAW_ATTRIB_FLATINLINE constexpr T const &operator*( ) const {
			DAW_ASSUME( validate( value ) );
			return value;
		}

		DAW_ATTRIB_FLATINLINE constexpr T const &get( ) const {
			DAW_ASSUME( validate( value ) );
			return value;
		}

		DAW_ATTRIB_FLATINLINE constexpr auto operator->( ) const {
			DAW_ASSUME( validate( value ) );
			if constexpr( std::is_pointer_v<T> ) {
				return value;
			} else {
				return std::addressof( value );
			}
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
		constexpr auto add_condition( Pre2s... ) const {
			static_assert(
			  ( daw::is_callable_r_v<bool, Pre2s, T> and ... ),
			  "Preconditions must be callable with T and the return must "
			  "convert to bool" );

			DAW_ASSUME( validate( value ) );
			return contract<T, Preconditions..., Pre2s...>{ value };
		}
	};
} // namespace daw