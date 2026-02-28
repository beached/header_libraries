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

namespace {
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
} // namespace

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
