// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cpp_feature_check.h"

#include <ciso646>
#include <cstring>
#include <type_traits>

#if defined( __cpp_lib_bit_cast ) and DAW_HAS_INCLUDE( <bit> )
#include <bit>
#endif

namespace daw {
	template<typename To, typename From>
	[[nodiscard, maybe_unused]] To
	bit_cast( From &&from ) noexcept( std::is_nothrow_constructible_v<To> ) {

		static_assert( std::is_trivially_copyable_v<
		                 std::remove_cv_t<std::remove_reference_t<From>>>,
		               "From To must be trivially copiable" );
		static_assert( std::is_trivially_copyable_v<
		                 std::remove_cv_t<std::remove_reference_t<To>>>,
		               "To To must be trivially copiable" );
		static_assert( sizeof( From ) == sizeof( To ),
		               "Sizes of From and To Tos must be the same" );
		static_assert( std::is_default_constructible_v<To>,
		               "To To must be default constructible" );

		auto result = std::aligned_storage_t<sizeof( To ), alignof( To )>{ };
		return *static_cast<To *>( std::memcpy( &result, &from, sizeof( To ) ) );
	}
} // namespace daw

#if defined( __cpp_lib_bit_cast )

#define DAW_BIT_CAST( To, ... ) std::bit_cast<To>( __VA_ARGS__ )
#define DAW_CX_BIT_CAST

#elif DAW_HAS_BUILTIN( __builtin_bit_cast ) or                                 \
  ( defined( __clang__ ) and ( __clang_major__ >= 9 ) )

#define DAW_BIT_CAST( To, ... ) __builtin_bit_cast( To, __VA_ARGS__ )
#define DAW_CX_BIT_CAST

#elif defined( _MSC_VER ) and _MSC_VER >= 1926

#define DAW_BIT_CAST( To, ... ) __builtin_bit_cast( To, __VA_ARGS__ )
#define DAW_CX_BIT_CAST

#else

#define DAW_BIT_CAST( To, ... ) daw::bit_cast<To>( __VA_ARGS__ )

#endif
