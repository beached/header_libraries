// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_unreachable.h"

#include <memory>

namespace daw {
	template<typename Obj, typename T>
	constexpr std::size_t offset_of( T Obj::*Mem ) {
		union {
			alignas( Obj ) char arr[sizeof( Obj )];
			Obj obj;
		} data{ };

		for( std::size_t i = 0; i < sizeof( Obj ); ++i ) {
			if( static_cast<const void *>( &data.arr[i] ) ==
			    static_cast<const void *>( std::addressof( data.obj.*Mem ) ) ) {
				return i;
			}
		}
		DAW_UNREACHABLE( );
	}
} // namespace daw
