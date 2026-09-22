// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <tuple>
#include <type_traits>

namespace daw {
	template<typename Function, typename... Params>
	class bind_front {
		Function m_func;
		std::tuple<Params...> m_params;

	public:
		template<typename F, typename... P>
		explicit constexpr bind_front( F &&func, P &&...params )
		  : m_func( DAW_FWD( func ) )
		  , m_params( DAW_FWD( params )... ) {}

		template<typename... Args>
		constexpr decltype( auto ) operator( )( Args &&...args ) {
			static_assert( std::is_invocable_v<Function, Params..., Args...>,
			               "Arguments are not valid for function" );
			return std::apply(
			  m_func,
			  std::tuple_cat( m_params,
			                  std::forward_as_tuple( DAW_FWD( args )... ) ) );
		}

		template<typename... Args>
		constexpr decltype( auto ) operator( )( Args &&...args ) const {
			static_assert( std::is_invocable_v<Function, Params..., Args...>,
			               "Arguments are not valid for function" );
			return std::apply(
			  m_func,
			  std::tuple_cat( m_params,
			                  std::forward_as_tuple( DAW_FWD( args )... ) ) );
		}
	};

	template<typename Function, typename... Params>
	bind_front( Function, Params... ) -> bind_front<Function, Params...>;
} // namespace daw
