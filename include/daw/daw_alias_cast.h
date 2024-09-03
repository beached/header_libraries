// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"

#include <type_traits>

namespace daw {
	/// Some types can come from a variety of type erase pointers.  This makes it
	/// easier so that only the To type is validated at compile time
	struct OnlyCheckToType;

	template<typename... AllowedTypes>
	struct fixed_cast {
		static constexpr bool only_check_to_type =
		  ( std::is_same_v<OnlyCheckToType, AllowedTypes> or ... );

		template<typename To, typename From>
		constexpr auto *operator( )( From *ptr ) const {
			static_assert( only_check_to_type or
			               ( std::is_same_v<From, AllowedTypes> or ... ) );
			static_assert( ( std::is_same_v<To, AllowedTypes> or ... ) );
			return reinterpret_cast<To *>( ptr );
		}
		template<typename To, typename From>
		constexpr auto *operator( )( From &ptr ) const {
			static_assert( only_check_to_type or
			               ( std::is_same_v<From, AllowedTypes> or ... ) );
			static_assert( ( std::is_same_v<To, AllowedTypes> or ... ) );
			return reinterpret_cast<To &>( ptr );
		}
	};
} // namespace daw
