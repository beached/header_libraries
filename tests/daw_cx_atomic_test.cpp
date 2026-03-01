// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_cx_atomic.h>
#include <daw/daw_ensure.h>

#include <concepts>
#include <type_traits>

namespace {
	consteval bool cx_store_load_exchange( ) {
		daw::atomic<float> value{ 1.0F };
		value.store( 2.0F );
		if( value.load( ) != 2.0F ) {
			return false;
		}
		auto const old = value.exchange( 3.0F );
		return old == 2.0F and value.load( ) == 3.0F;
	}

	consteval bool cx_compare_exchange_success( ) {
		daw::atomic<float> value{ 10.0F };
		auto expected = 10.0F;
		auto const result = value.compare_exchange_strong( expected, 22.0F );
		return result and expected == 10.0F and value.load( ) == 22.0F;
	}

	consteval bool cx_compare_exchange_failure_reports_failure( ) {
		daw::atomic<float> value{ 9.0F };
		auto expected = 8.0F;
		auto const result = value.compare_exchange_strong( expected, 13.0F );
		return ( not result ) and expected == 9.0F and value.load( ) == 9.0F;
	}

	consteval bool cx_fetch_and_operator_semantics( ) {
		daw::atomic<int> v{ 10 };

		if( v.fetch_add( 5 ) != 10 ) {
			return false;
		}
		if( v.load( ) != 15 ) {
			return false;
		}
		if( v.fetch_sub( 2 ) != 15 ) {
			return false;
		}
		if( v.load( ) != 13 ) {
			return false;
		}

		if( v.fetch_and( 12 ) != 13 ) { // 1101 & 1100 -> 1100 (12)
			return false;
		}
		if( v.fetch_or( 3 ) != 12 ) { // 1100 | 0011 -> 1111 (15)
			return false;
		}
		return v.fetch_xor( 5 ) == 15 and // 1111 ^ 0101 -> 1010 (10)
		       v.load( ) == 10;
	}

	void runtime_store_load_exchange( ) {
		auto value = daw::atomic<float>{ 0.25F };
		daw_ensure( value.load( ) == 0.25F );

		value.store( 1.5F, std::memory_order_release );
		daw_ensure( value.load( std::memory_order_acquire ) == 1.5F );

		auto const old = value.exchange( 4.0F, std::memory_order_acq_rel );
		daw_ensure( old == 1.5F );
		daw_ensure( value.load( ) == 4.0F );
	}

	void runtime_compare_exchange_overloads( ) {
		auto value = daw::atomic<float>{ 3.0F };

		{
			auto expected = 3.0F;
			auto const result = value.compare_exchange_strong(
			  expected, 8.0F, std::memory_order_acq_rel, std::memory_order_acquire );
			daw_ensure( result );
			daw_ensure( expected == 3.0F );
			daw_ensure( value.load( ) == 8.0F );
		}

		{
			auto expected = 7.0F;
			auto const result = value.compare_exchange_weak(
			  expected, 9.0F, std::memory_order_acq_rel );
			daw_ensure( not result );
			daw_ensure( expected == 8.0F );
			daw_ensure( value.load( ) == 8.0F );
		}
	}
	void runtime_int_basic_and_cas( ) {
		daw::atomic<int> v{ 1 };
		daw_ensure( v.load( ) == 1 );
		v.store( 2 );
		daw_ensure( v.load( ) == 2 );
		daw_ensure( v.exchange( 3 ) == 2 );
		daw_ensure( v.load( ) == 3 );

		int expected = 3;
		daw_ensure( v.compare_exchange_strong( expected, 9 ) );
		daw_ensure( v.load( ) == 9 );

		expected = 1;
		daw_ensure( not v.compare_exchange_weak( expected, 10 ) );
		daw_ensure( expected == 9 );
	}

	void runtime_int_arithmetic_and_bitwise( ) {
		daw::atomic<int> v{ 10 };

		daw_ensure( v.fetch_add( 5 ) == 10 );
		daw_ensure( v.load( ) == 15 );
		daw_ensure( v.fetch_sub( 2 ) == 15 );
		daw_ensure( v.load( ) == 13 );

		daw_ensure( ( v += 7 ) == 20 );
		daw_ensure( ( v -= 4 ) == 16 );
		daw_ensure( ++v == 17 );
		daw_ensure( v++ == 17 );
		daw_ensure( v.load( ) == 18 );
		daw_ensure( --v == 17 );
		daw_ensure( v-- == 17 );
		daw_ensure( v.load( ) == 16 );

		daw_ensure( v.fetch_and( 12 ) == 16 ); // 10000 & 01100 -> 0
		daw_ensure( v.load( ) == 0 );
		daw_ensure( v.fetch_or( 3 ) == 0 ); // 00000 | 00011 -> 3
		daw_ensure( v.load( ) == 3 );
		daw_ensure( v.fetch_xor( 6 ) == 3 ); // 00011 ^ 00110 -> 5
		daw_ensure( v.load( ) == 5 );

		daw_ensure( ( v &= 7 ) == 5 );
		daw_ensure( ( v |= 8 ) == 13 );
		daw_ensure( ( v ^= 3 ) == 14 );
		daw_ensure( v.load( ) == 14 );
	}
} // namespace

int main( ) {
	static_assert( daw::AtomicValue<float> );
	static_assert( not daw::AtomicValue<void> );
	static_assert( cx_store_load_exchange( ) );
	static_assert( cx_compare_exchange_success( ) );
	static_assert( cx_fetch_and_operator_semantics( ) );
	static_assert( cx_compare_exchange_failure_reports_failure( ) );

	static_assert( requires( daw::atomic<int> a ) {
		a.store( 1 );
		{ a.load( ) }->std::same_as<int>;
	} );
	static_assert( requires( daw::atomic<int> a ) {
		a.fetch_add( 1 );
		a.fetch_sub( 1 );
		a.fetch_and( 1 );
		a.fetch_or( 1 );
		a.fetch_xor( 1 );
		++a;
		a++;
	} );

	runtime_store_load_exchange( );
	runtime_compare_exchange_overloads( );
	runtime_int_basic_and_cas( );
	runtime_int_arithmetic_and_bitwise( ); // float should NOT have ++ or --
}
