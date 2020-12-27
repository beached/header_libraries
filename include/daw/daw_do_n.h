// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_hide.h"

namespace daw::algorithm {
	namespace do_n_details DAW_ATTRIBUTE_HIDDEN {
		template<typename Function, typename... Args, std::size_t... Is>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr void
		do_n( Function &&func, std::integer_sequence<std::size_t, Is...>,
		      Args &&...args ) noexcept( noexcept( func( args... ) ) ) {
			if constexpr( sizeof...( Is ) > 0 ) {
				(void)( ( func( args... ), Is ) + ... );
			}
		}

		template<std::size_t by_n, typename Function, std::size_t... Is>
		DAW_ATTRIBUTE_FLATTEN static inline constexpr void do_n_arg(
		  Function &&func,
		  std::integer_sequence<std::size_t,
		                        Is...> ) noexcept( noexcept( func( 0ULL ) ) ) {
			if constexpr( sizeof...( Is ) > 0 ) {
				(void)( ( func( Is * by_n ), 0 ) + ... );
			}
		}
	} // namespace DAW_ATTRIBUTE_HIDDEN

	template<std::size_t count, typename Function, typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void do_n( Function &&func,
	                                                         Args &&...args ) {
		do_n_details::do_n( std::forward<Function>( func ),
		                    std::make_integer_sequence<std::size_t, count>{ },
		                    std::forward<Args>( args )... );
	}

	template<typename Function, typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	do_n( std::size_t count, Function &&func, Args &&...args ) noexcept(
	  std::is_nothrow_invocable_v<Function, Args...> ) {
		while( count-- > 0 ) {
			func( args... );
		}
	}

	template<typename Function, typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	do_n_arg( std::size_t count, Function &&func ) noexcept(
	  std::is_nothrow_invocable_v<Function, std::size_t> ) {
		std::size_t n = 0;
		while( n < count ) {
			func( n++ );
		}
	}

	template<std::size_t count, std::size_t by_n = 1, typename Function,
	         typename... Args>
	DAW_ATTRIBUTE_FLATTEN static inline constexpr void
	do_n_arg( Function &&func ) noexcept(
	  std::is_nothrow_invocable_v<Function, std::size_t> ) {
		static_assert( by_n > 0 );
		do_n_details::do_n_arg<by_n>( std::forward<Function>( func ),
		                              std::make_index_sequence<count / by_n>{ } );
	}
} // namespace daw::algorithm
