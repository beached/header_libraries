// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <ciso646>
#include <cstddef>
#include <type_traits>
#include <utility>

#if defined( _MSC_VER ) and not defined( __clang__ )
#include <intrin.h>
#endif

namespace daw {
#if not defined( _MSC_VER ) or defined( __clang__ )
	template<typename Tp>
	inline void do_not_optimize( Tp const &value ) {
		asm volatile( "" : : "r,m"( value ) : "memory" );
	}

	template<typename Tp>
	inline void do_not_optimize( Tp &value ) {
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
#pragma warning( disable: 4702 )
	template<class T>
	inline void do_not_optimize( T const &value ) {
		do_not_optimize_details::UseCharPointer(
		  &reinterpret_cast<char const volatile &>( value ) );
		_ReadWriteBarrier( );
	}
#pragma optimize( "", on )
#pragma warning( default: 4702 )
#endif

	template<typename... Values, std::enable_if_t<( sizeof...( Values ) != 1 ),
	                                              std::nullptr_t> = nullptr>
	void do_not_optimize( Values &&...values ) {
		if constexpr( sizeof...( Values ) > 0 ) {
			(void)( ( do_not_optimize( std::forward<Values>( values ) ), 1 ) | ... );
		}
	}
} // namespace daw
