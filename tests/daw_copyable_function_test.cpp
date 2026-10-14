// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_copyable_function.h"
#include "daw/daw_ensure.h"

#include <type_traits>
#include <utility>

namespace {
	struct ref_qualified {
		int operator( )( ) & {
			return 1;
		}

		int operator( )( ) && {
			return 2;
		}
	};

	struct const_ref_qualified {
		int operator( )( ) const & {
			return 3;
		}

		int operator( )( ) const && {
			return 4;
		}
	};

	struct in_place_callable {
		int value;

		explicit in_place_callable( int v )
		  : value( v ) {}

		int operator( )( ) const {
			return value;
		}
	};

	int increment( int value ) {
		return value + 1;
	}
} // namespace

using unqualified_function = daw::copyable_function<int( )>;
using lvalue_function = daw::copyable_function<int( ) &>;
using rvalue_function = daw::copyable_function<int( ) &&>;
using const_function = daw::copyable_function<int( ) const>;

static_assert( std::is_copy_constructible_v<unqualified_function> );
static_assert( std::is_move_constructible_v<unqualified_function> );
static_assert( std::is_invocable_v<unqualified_function &> );
static_assert( std::is_invocable_v<unqualified_function &&> );
static_assert( not std::is_invocable_v<unqualified_function const &> );
static_assert( std::is_invocable_v<const_function const &> );
static_assert( std::is_invocable_v<lvalue_function &> );
static_assert( not std::is_invocable_v<lvalue_function &&> );
static_assert( not std::is_invocable_v<rvalue_function &> );
static_assert( std::is_invocable_v<rvalue_function &&> );
static_assert( std::is_constructible_v<lvalue_function, ref_qualified> );
static_assert( std::is_constructible_v<rvalue_function, ref_qualified> );
static_assert( not std::is_constructible_v<const_function, ref_qualified> );

int main( ) {
	daw::copyable_function<int( )> first = [count = 0]( ) mutable {
		return ++count;
	};
	daw_ensure( first( ) == 1 );

	auto second = first;
	daw_ensure( first( ) == 2 );
	daw_ensure( second( ) == 2 );

	auto moved = std::move( second );
	daw_ensure( moved( ) == 3 );
	daw_ensure( second == nullptr );

	daw::copyable_function<int( int )> pointer = &increment;
	daw_ensure( pointer( 41 ) == 42 );
	pointer = static_cast<int ( * )( int )>( nullptr );
	daw_ensure( pointer == nullptr );

	daw::copyable_function<int( )> in_place(
	  std::in_place_type<in_place_callable>, 42 );
	daw_ensure( in_place( ) == 42 );

	daw::copyable_function<int( ) &> lvalue = ref_qualified{ };
	daw_ensure( lvalue( ) == 1 );

	daw::copyable_function<int( ) &&> rvalue = ref_qualified{ };
	daw_ensure( std::move( rvalue )( ) == 2 );

	daw::copyable_function<int( ) const &> const_lvalue = const_ref_qualified{ };
	daw_ensure( std::as_const( const_lvalue )( ) == 3 );

	daw::copyable_function<int( ) const &&> const_rvalue = const_ref_qualified{ };
	daw_ensure( std::move( std::as_const( const_rvalue ) )( ) == 4 );

	daw::copyable_function<int( ) noexcept> no_throw = []( ) noexcept {
		return 42;
	};
	static_assert( noexcept( no_throw( ) ) );
	daw_ensure( no_throw( ) == 42 );

	daw::copyable_function<void( )> discard_result = [] {
		return 42;
	};
	discard_result( );
}
