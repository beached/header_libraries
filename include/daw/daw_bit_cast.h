// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_aligned_storage.h"
#include "daw_cpp_feature_check.h"

#include <array>
#include <cstring>
#include <type_traits>

#if not defined( DAW_HAS_CPP20_BIT_CAST ) and \
  defined( __cpp_lib_bit_cast ) and DAW_HAS_INCLUDE( <bit> )
#if __cpp_lib_bit_cast >= 201806L
#define DAW_HAS_CPP20_BIT_CAST
#endif
#endif

#if defined( DAW_HAS_CPP20_BIT_CAST )
#include <bit>
#endif

namespace daw {
	template<typename To, typename From>
	[[nodiscard]] To bit_cast( From &&from ) noexcept(
	  std::is_nothrow_default_constructible_v<To> ) {

		static_assert( std::is_trivially_copyable_v<
		                 std::remove_cv_t<std::remove_reference_t<From>>>,
		               "From To must be trivially copiable" );
		static_assert( std::is_trivially_copyable_v<
		                 std::remove_cv_t<std::remove_reference_t<To>>>,
		               "To To must be trivially copiable" );
		static_assert( sizeof( From ) == sizeof( To ),
		               "Sizes of From and To Tos must be the same" );
		static_assert( std::is_default_constructible_v<To>,
		               "To must be default constructible" );

		auto result = aligned_storage_for_t<To>{ };
		return *static_cast<To *>( std::memcpy( &result, &from, sizeof( To ) ) );
	}
} // namespace daw

#if defined( DAW_HAS_CPP20_BIT_CAST )

#define DAW_BIT_CAST( To, ... ) std::bit_cast<To>( __VA_ARGS__ )
#define DAW_CX_BIT_CAST

#elif not defined( DAW_HAS_MSVC ) and \
  ( DAW_HAS_BUILTIN( __builtin_bit_cast ) or DAW_HAS_CLANG_VER_GTE( 9, 0 ) )

#define DAW_BIT_CAST( To, ... ) __builtin_bit_cast( To, __VA_ARGS__ )
#define DAW_CX_BIT_CAST

#elif DAW_HAS_MSVC_VER_GTE( 1926 )
namespace daw::bit_cast_impl {
	template<typename To, typename From>
	inline constexpr To ms_bit_cast( From &&f ) {
		return __builtin_bit_cast( To, f );
	}
} // namespace daw::bit_cast_impl
#define DAW_BIT_CAST( To, ... ) \
	::daw::bit_cast_impl::ms_bit_cast<To>( __VA_ARGS__ )

#define DAW_CX_BIT_CAST

#else

#define DAW_BIT_CAST( To, ... ) daw::bit_cast<To>( __VA_ARGS__ )

#endif
