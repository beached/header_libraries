// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once
#include <cstdio>
#include <utility>

#include "daw_exchange.h"

namespace daw {
	class CFile {
		FILE *ptr = nullptr;

	public:
		constexpr CFile( ) noexcept = default;
		explicit constexpr CFile( FILE *f ) noexcept
		  : ptr( f ) {}

		constexpr CFile( CFile &&other ) noexcept
		  : ptr( exchange( other.ptr, nullptr ) ) {}

		constexpr CFile &operator=( CFile &&rhs ) noexcept {
			if( this != &rhs ) {
				ptr = exchange( rhs.ptr, nullptr );
			}
			return *this;
		}

		constexpr FILE *get( ) const noexcept {
			return ptr;
		}
		constexpr FILE *release( ) noexcept {
			return exchange( ptr, nullptr );
		}

		constexpr void close( ) {
			if( auto tmp = exchange( ptr, nullptr ); tmp ) {
				fclose( tmp );
			}
		}
		~CFile( ) {
			close( );
		}
	};
} // namespace daw
