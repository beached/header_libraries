// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_attributes.h"
#include "daw_cpp_feature_check.h"
#include "daw_move.h"

#include <cstddef>
#include <type_traits>

#if defined( DAW_HAS_MSVC )
#include <intrin.h>
#endif

namespace daw {
#if defined( DAW_HAS_GCC_LIKE )
	template<typename Tp>
	DAW_ATTRIB_INLINE void do_not_optimize( Tp const &value ) {
		asm volatile( "" : : "r,m"( value ) : "memory" );
	}

	template<typename Tp>
	DAW_ATTRIB_INLINE void do_not_optimize( Tp &value ) {
#if defined( __clang__ )
		asm volatile( "" : "+r,m"( value ) : : "memory" );
#else
		asm volatile( "" : "+m,r"( value ) : : "memory" );
#endif
	}
#else
	namespace do_not_optimize_details {
		[[maybe_unused]] constexpr void UseCharPointer( char const volatile * ) {}
	} // namespace do_not_optimize_details

#pragma optimize( "", off )
#pragma warning( disable : 4702 )
	template<class T>
	DAW_ATTRIB_INLINE void do_not_optimize( T const &value ) {
		do_not_optimize_details::UseCharPointer(
		  &reinterpret_cast<char const volatile &>( value ) );
		_ReadWriteBarrier( );
	}
#pragma optimize( "", on )
#pragma warning( default : 4702 )
#endif

	template<typename... Values, std::enable_if_t<( sizeof...( Values ) != 1 ),
	                                              std::nullptr_t> = nullptr>
	DAW_ATTRIB_INLINE void do_not_optimize( Values &&...values ) {
		if constexpr( sizeof...( Values ) > 0 ) {
			(void)( ( do_not_optimize( DAW_FWD( values ) ), 1 ) | ... );
		}
	}
} // namespace daw
