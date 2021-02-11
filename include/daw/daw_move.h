// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <type_traits>

namespace daw {
	/// Convert a value to an rvalue.
	/// \param  value  A thing of arbitrary type.
	/// \return The parameter cast to an rvalue-reference to allow moving it.
	template<typename T>
	[[nodiscard]] inline constexpr typename std::remove_reference<T>::type &&
	move( T &&value ) noexcept {
		static_assert(
		  not std::is_const<typename std::remove_reference<T>::type>::value,
		  "Attempt to move const value" );
		static_assert( not std::is_rvalue_reference<decltype( value )>::value,
		               "Value is already an rvalue" );
		return static_cast<typename std::remove_reference<T>::type &&>( value );
	}
} // namespace daw

#ifndef DAW_FWD
#define DAW_FWD( ... ) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)
#endif

