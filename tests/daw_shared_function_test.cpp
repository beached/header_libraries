// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_shared_function.h>

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

struct lvalue_incrementer_t {
	int delta = 0;

	int operator( )( int value ) const {
		return value + delta;
	}
};

struct lvalue_noexcept_functor_t {
	int operator( )( ) const noexcept {
		return 42;
	}
};

int increment( int value ) {
	return value + 1;
}

struct cx_functor_t {
	int delta = 0;
	constexpr int operator( )( int v ) const {
		return v + delta;
	}
};

void test_basic_and_copy_semantics( ) {
	auto sf0 = daw::shared_function<int( )>( [count = 0]( ) mutable {
		return ++count;
	} );
	daw_ensure( sf0( ) == 1 );

	auto sf1 = sf0;
	daw_ensure( sf1( ) == 2 );
	daw_ensure( sf0( ) == 3 );
}

void test_move_semantics_and_bool( ) {
	auto sf0 = daw::shared_function<int( )>( [] {
		return 42;
	} );
	auto sf1 = std::move( sf0 );

	daw_ensure( static_cast<bool>( sf1 ) );
	daw_ensure( not static_cast<bool>( sf0 ) );
	daw_ensure( sf1( ) == 42 );
}

void test_noexcept_signature( ) {
	auto sf = daw::shared_function<int( ) noexcept>( [] noexcept {
		return 7;
	} );
	static_assert( noexcept( sf( ) ) );
	daw_ensure( sf( ) == 7 );
}

void test_const_signature( ) {
	auto const sf = daw::shared_function<int( int ) const>( []( int v ) {
		return v * 2;
	} );
	daw_ensure( sf( 21 ) == 42 );
}

void test_lvalue_ref_qualified_signature( ) {
	auto sf = daw::shared_function<int( ) &>( [count = 0]( ) mutable {
		return ++count;
	} );
	daw_ensure( sf( ) == 1 );

	auto sf_copy = sf;
	daw_ensure( sf_copy( ) == 2 );
	daw_ensure( sf( ) == 3 );
}

void test_rvalue_ref_qualified_signature( ) {
	auto sf = daw::shared_function<int( ) &&>( [count = 0]( ) mutable {
		return ++count;
	} );
	daw_ensure( std::move( sf )( ) == 1 );
	daw_ensure( std::move( sf )( ) == 2 );
}

void test_function_pointer_support( ) {
	auto sf = daw::shared_function<int( int )>( increment );
	daw_ensure( sf( 41 ) == 42 );
}

void test_lvalue_functor_support( ) {
	auto fn = lvalue_incrementer_t{ .delta = 2 };
	auto sf = daw::shared_function<int( int )>( fn );
	daw_ensure( sf( 40 ) == 42 );
}

void test_lvalue_noexcept_functor_support( ) {
	auto fn = lvalue_noexcept_functor_t{ };
	auto sf = daw::shared_function<int( ) noexcept>( fn );
	static_assert( noexcept( sf( ) ) );
	daw_ensure( sf( ) == 42 );
}

void test_weak_lock( ) {
	auto sf = daw::shared_function<int( )>( [] {
		return 42;
	} );
	auto weak = sf.get_weak( );
	auto locked = weak.lock( );
	daw_ensure( locked( ) == 42 );
	daw_ensure( sf( ) == 42 );
}

void test_move_only_parameter( ) {
	auto sf = daw::shared_function<int( std::unique_ptr<int> )>(
	  []( std::unique_ptr<int> p ) {
		  return *p;
	  } );
	daw_ensure( sf( std::make_unique<int>( 42 ) ) == 42 );
}
// constexpr tests

// Basic call
static_assert( [] {
	auto sf = daw::shared_function<int( )>( [] {
		return 42;
	} );
	return sf( ) == 42;
}( ) );

// Call with parameters
static_assert( [] {
	auto sf = daw::shared_function<int( int, int )>( []( int a, int b ) {
		return a + b;
	} );
	return sf( 20, 22 ) == 42;
}( ) );

// Copies share the same callable (same storage)
static_assert( [] {
	auto sf0 = daw::shared_function<int( int )>( []( int v ) {
		return v * 2;
	} );
	auto sf1 = sf0;
	return sf0( 21 ) == 42 and sf1( 21 ) == 42;
}( ) );

// Mutable shared state: copies all see the same counter
struct CXTest_t {
	int count = 0;
	constexpr int operator( )( ) {
		return ++count;
	}
};
static_assert( [] {
	auto sf0 = daw::shared_function<int( )>( CXTest_t{ } );
	auto r0 = sf0( );
	auto sf1 = sf0;
	auto r1 = sf1( );
	auto r2 = sf0( );
	return r0 == 1 and r1 == 2 and r2 == 3;
}( ) );

// Move: source becomes empty
static_assert( [] {
	auto sf0 = daw::shared_function<int( )>( [] {
		return 42;
	} );
	auto sf1 = std::move( sf0 );
	return not static_cast<bool>( sf0 ) and static_cast<bool>( sf1 ) and
	       sf1( ) == 42;
}( ) );

// nullptr construction yields an empty (false) object
static_assert( [] {
	daw::shared_function<int( )> sf( nullptr );
	return not static_cast<bool>( sf );
}( ) );

// noexcept signature
static_assert( [] {
	auto sf = daw::shared_function<int( ) noexcept>( [] noexcept {
		return 7;
	} );
	return sf( ) == 7;
}( ) );

// const-qualified signature
static_assert( [] {
	auto const sf = daw::shared_function<int( int ) const>( []( int v ) {
		return v + 1;
	} );
	return sf( 41 ) == 42;
}( ) );

// const noexcept signature
static_assert( [] {
	auto const sf = daw::shared_function<int( ) const noexcept>( [] noexcept {
		return 42;
	} );
	return sf( ) == 42;
}( ) );

// Functor type
static_assert( [] {
	auto sf = daw::shared_function<int( int )>( cx_functor_t{ .delta = 2 } );
	return sf( 40 ) == 42;
}( ) );

// Copy assignment
static_assert( [] {
	auto sf0 = daw::shared_function<int( )>( [] {
		return 1;
	} );
	auto sf1 = daw::shared_function<int( )>( [] {
		return 2;
	} );
	sf1 = sf0;
	return sf0( ) == 1 and sf1( ) == 1;
}( ) );

// Move assignment
static_assert( [] {
	auto sf0 = daw::shared_function<int( )>( [] {
		return 42;
	} );
	daw::shared_function<int( )> sf1( nullptr );
	sf1 = std::move( sf0 );
	return not static_cast<bool>( sf0 ) and sf1( ) == 42;
}( ) );

// Weak lock: locked copy calls the same function
static_assert( [] {
	auto sf = daw::shared_function<int( )>( [] {
		return 42;
	} );
	auto weak = sf.get_weak( );
	auto locked = weak.lock( );
	return static_cast<bool>( locked ) and locked( ) == 42;
}( ) );

int main( ) {
	test_basic_and_copy_semantics( );
	test_move_semantics_and_bool( );
	test_noexcept_signature( );
	test_const_signature( );
	test_lvalue_ref_qualified_signature( );
	test_rvalue_ref_qualified_signature( );
	test_function_pointer_support( );
	test_lvalue_functor_support( );
	test_lvalue_noexcept_functor_support( );
	test_weak_lock( );
	test_move_only_parameter( );

	{
		auto s0 = daw::shared_function<int( )>( [] {
			return 42;
		} );
		daw_ensure( s0( ) == 42 );
		{
			auto s1 = s0;
			daw_ensure( s1( ) == 42 );
		}
		daw_ensure( s0( ) == 42 );
	}
	{
		auto s0 = daw::shared_function<int( ) noexcept>( [] {
			return 42;
		} );
		daw_ensure( s0( ) == 42 );
		{
			auto s1 = s0;
			daw_ensure( s1( ) == 42 );
		}
		daw_ensure( s0( ) == 42 );
	}

	{
		int x = 5;
		auto const s0 = daw::shared_function<int( ) const>( [x]( ) {
			return x + 37;
		} );
		daw_ensure( s0( ) == 42 );
		{
			auto s1 = s0;
			daw_ensure( s1( ) == 42 );
		}
		daw_ensure( s0( ) == 42 );
	}
	{
		auto s0 = daw::shared_function<int( ) const noexcept>( [] {
			return 42;
		} );
		daw_ensure( s0( ) == 42 );
		{
			auto s1 = s0;
			daw_ensure( s1( ) == 42 );
		}
		daw_ensure( s0( ) == 42 );
	}
}
