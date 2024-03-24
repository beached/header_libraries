// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_exchange.h"

#include <cstdio>
#include <utility>

namespace daw {
	class CFile {
		FILE *ptr = nullptr;

	public:
		CFile( ) = default;
		CFile( CFile const & ) = delete;
		CFile &operator=( CFile const & ) = delete;

		explicit constexpr CFile( FILE *f ) noexcept
		  : ptr( f ) {}

		constexpr CFile( CFile &&other ) noexcept
		  : ptr( exchange( other.ptr, nullptr ) ) {}

		constexpr CFile &operator=( CFile &&rhs ) noexcept {
			reset( release( ) );
			return *this;
		}

		~CFile( ) {
			close( );
		}

		constexpr FILE *get( ) const noexcept {
			return ptr;
		}

		constexpr FILE *release( ) noexcept {
			return exchange( ptr, nullptr );
		}

		constexpr void reset( FILE *f ) noexcept {
			if( ptr ) {
				close( );
			}
			ptr = f;
		}

		constexpr void close( ) noexcept {
			if( auto tmp = exchange( ptr, nullptr ); tmp ) {
				fclose( tmp );
			}
		}

		constexpr void reset( ) noexcept {
			close( );
		}
	};
} // namespace daw
