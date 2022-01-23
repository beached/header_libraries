// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../daw_move.h"
#include "daw_function_iterator.h"

#include <ciso646>
#include <deque>
#include <functional>
#include <list>
#include <vector>

namespace daw {
	namespace si_impl {
		// Workaround
		// MSVC had issues with lambdas
		template<typename Compare, typename... VecArgs>
		struct sorted_iterator_vector_callable {
			std::vector<VecArgs...> *ptr;
			Compare compare;

			template<typename T>
			constexpr void operator( )( T &&value ) const {
				auto pos = std::lower_bound( std::begin( *ptr ), std::end( *ptr ),
				                             value, compare );
				ptr->emplace( DAW_MOVE( pos ),
				              std::forward<decltype( value )>( value ) );
			}
		};

		template<typename Compare, typename... VecArgs>
		struct sorted_iterator_deque_callable {
			std::deque<VecArgs...> *ptr;
			Compare compare;

			template<typename T>
			constexpr void operator( )( T &&value ) const {
				auto pos = std::lower_bound( std::begin( *ptr ), std::end( *ptr ),
				                             value, compare );
				ptr->emplace( DAW_MOVE( pos ),
				              std::forward<decltype( value )>( value ) );
			}
		};

		template<typename Compare, typename... VecArgs>
		struct sorted_iterator_list_callable {
			std::list<VecArgs...> *ptr;
			Compare compare;

			template<typename T>
			constexpr void operator( )( T &&value ) const {
				auto const pos = std::lower_bound( std::begin( *ptr ), std::end( *ptr ),
				                                   value, compare );
				ptr->emplace( DAW_MOVE( pos ),
				              std::forward<decltype( value )>( value ) );
			}
		};

	} // namespace si_impl

	template<typename... Args, typename Compare = std::less<>>
	auto make_sorted_insert_iterator( std::vector<Args...> &c,
	                                  Compare compare = Compare{ } ) {
		return daw::make_function_iterator(
		  si_impl::sorted_iterator_vector_callable<Compare, Args...>{
		    &c, DAW_MOVE( compare ) } );
	}

	template<typename... Args, typename Compare = std::less<>>
	auto make_sorted_insert_iterator( std::deque<Args...> &c,
	                                  Compare compare = Compare{ } ) {
		return daw::make_function_iterator(
		  si_impl::sorted_iterator_deque_callable<Compare, Args...>{
		    &c, DAW_MOVE( compare ) } );
	}

	template<typename... Args, typename Compare = std::less<>>
	decltype( auto ) make_sorted_insert_iterator( std::list<Args...> &c,
	                                              Compare compare = Compare{ } ) {
		return daw::make_function_iterator(
		  si_impl::sorted_iterator_list_callable<Compare, Args...>{
		    &c, DAW_MOVE( compare ) } );
	}
} // namespace daw
