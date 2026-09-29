// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/pipelines/nonpropagating_cache.h"

#include <daw/daw_ensure.h>

#include <concepts>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using int_cache = daw::nonpropagating_cache<int>;

static_assert( std::default_initializable<int_cache> );
static_assert( std::is_nothrow_copy_constructible_v<int_cache> );
static_assert( std::is_nothrow_move_constructible_v<int_cache> );
static_assert( std::is_nothrow_copy_assignable_v<int_cache> );
static_assert( std::is_nothrow_move_assignable_v<int_cache> );
static_assert( std::equality_comparable<int_cache> );

// An rvalue cache that owns its value moves it out
static_assert(
  std::is_same_v<decltype( std::declval<daw::nonpropagating_cache<std::string> &&>( ).get( ) ),
                 std::string> );
static_assert(
  std::is_same_v<decltype( std::declval<daw::nonpropagating_cache<std::string> &>( ).get( ) ),
                 std::string &> );

// Filling a const cache is not allowed, it would not be thread safe.  The
// check needs a template context to be false rather than an error
template<typename Cache>
inline constexpr bool can_fill =
  requires( Cache &c ) { c.set_if_empty_get( [] { return 1; } ); };
static_assert( can_fill<int_cache> );
static_assert( not can_fill<int_cache const> );

constexpr bool test_starts_empty( ) {
	int_cache c = { };
	return not c.has_value( ) and not static_cast<bool>( c );
}
static_assert( test_starts_empty( ) );

constexpr bool test_set_if_empty_get_fills_once( ) {
	auto c = int_cache{ };
	int calls = 0;
	auto make = [&] {
		++calls;
		return 42;
	};
	auto const first = c.set_if_empty_get( make );
	auto const second = c.set_if_empty_get( make );
	return first == 42 and second == 42 and calls == 1 and c.has_value( );
}
static_assert( test_set_if_empty_get_fills_once( ) );

constexpr bool test_copy_does_not_propagate( ) {
	auto c = int_cache{ };
	(void)c.set_if_empty_get( [] { return 1; } );
	auto copy = c;
	return not copy.has_value( ) and c.has_value( );
}
static_assert( test_copy_does_not_propagate( ) );

constexpr bool test_move_empties_both( ) {
	auto c = int_cache{ };
	(void)c.set_if_empty_get( [] { return 1; } );
	auto moved = std::move( c );
	return not moved.has_value( ) and not c.has_value( );
}
static_assert( test_move_empties_both( ) );

constexpr bool test_copy_assign_empties_target( ) {
	auto source = int_cache{ };
	(void)source.set_if_empty_get( [] { return 1; } );
	auto target = int_cache{ };
	(void)target.set_if_empty_get( [] { return 2; } );
	target = source;
	return not target.has_value( ) and source.has_value( );
}
static_assert( test_copy_assign_empties_target( ) );

constexpr bool test_self_copy_assign_keeps_value( ) {
	auto c = int_cache{ };
	(void)c.set_if_empty_get( [] { return 7; } );
	auto const &same = c;
	c = same;
	return c.has_value( ) and c.get( ) == 7;
}
static_assert( test_self_copy_assign_keeps_value( ) );

constexpr bool test_move_assign_empties_both( ) {
	auto source = int_cache{ };
	(void)source.set_if_empty_get( [] { return 1; } );
	auto target = int_cache{ };
	(void)target.set_if_empty_get( [] { return 2; } );
	target = std::move( source );
	return not target.has_value( ) and not source.has_value( );
}
static_assert( test_move_assign_empties_both( ) );

constexpr bool test_reset( ) {
	auto c = int_cache{ };
	(void)c.set_if_empty_get( [] { return 1; } );
	c.reset( );
	return not c.has_value( );
}
static_assert( test_reset( ) );

// All caches compare equal, so a type holding one can default operator==
struct holder {
	int value = 0;
	int_cache cache = { };
	constexpr bool operator==( holder const & ) const = default;
};

constexpr bool test_equality( ) {
	auto a = holder{ 1 };
	auto b = holder{ 1 };
	(void)a.cache.set_if_empty_get( [] { return 5; } );
	return a == b and a.cache == b.cache and not( holder{ 1 } == holder{ 2 } );
}
static_assert( test_equality( ) );

int main( ) {
	auto v = std::vector<int>{ 10, 20, 30 };

	// A reference cache refers to the element
	auto ref = daw::nonpropagating_cache<int &>{ };
	auto &element = ref.set_if_empty_get( [&]( ) -> int & { return v[0]; } );
	daw_ensure( &element == &v[0] );
	ref.get( ) = 99;
	daw_ensure( v[0] == 99 );

	// After a reset the cache is filled again
	auto c = daw::nonpropagating_cache<int>{ };
	daw_ensure( c.set_if_empty_get( [] { return 20; } ) == 20 );
	c.reset( );
	daw_ensure( c.set_if_empty_get( [] { return 30; } ) == 30 );

	// Caching an iterator, as a view would cache begin( )
	auto it_cache = daw::nonpropagating_cache<std::vector<int>::iterator>{ };
	auto const &cached = it_cache.set_if_empty_get( [&] { return v.begin( ) + 1; } );
	daw_ensure( *cached == 20 );

	// An rvalue cache moves its value out
	auto s = daw::nonpropagating_cache<std::string>{ };
	(void)s.set_if_empty_get( [] { return std::string( 64, 'x' ); } );
	auto const taken = std::move( s ).get( );
	daw_ensure( taken.size( ) == 64 );
}
