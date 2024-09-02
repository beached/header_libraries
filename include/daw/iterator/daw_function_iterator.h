// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/cpp_17.h"
#include "daw/daw_algorithm.h"
#include "daw/daw_move.h"
#include "daw/daw_traits.h"

namespace daw {
	///
	///	An output iterator that calls supplied function when operator= is called
	/// This is like std::back_insert_iterator
	///
	template<typename Function>
	struct function_iterator {
		using iterator_category = std::output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	private:
		Function m_function;

	public:
		explicit constexpr function_iterator( Function function )
		  : m_function( std::move( function ) ) {}

		template<typename T,
		         std::enable_if_t<
		           not std::is_same_v<daw::remove_cvref_t<T>, function_iterator>,
		           std::nullptr_t> = nullptr>
		constexpr function_iterator &operator=( T &&val ) {
			(void)m_function( DAW_FWD( val ) );
			return *this;
		}

		constexpr function_iterator &operator*( ) noexcept {
			return *this;
		}

		constexpr function_iterator &operator++( ) noexcept {
			return *this;
		}

		constexpr function_iterator &operator++( int ) noexcept {
			return *this;
		}
	};
	template<typename Function>
	function_iterator( Function ) -> function_iterator<Function>;

	///
	/// Create a function_iterator with supplied function
	///
	template<typename Function>
	constexpr auto make_function_iterator( Function &&func ) {
		return function_iterator<Function>( DAW_FWD( func ) );
	}
} // namespace daw
