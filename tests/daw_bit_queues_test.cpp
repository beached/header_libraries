// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/daw_bit_queues.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

bool daw_bit_queues_test_001( ) {
	using value_type = uint16_t;
	daw::basic_bit_queue<value_type, value_type> test1;
	test1.push_back( 1, 1 );
	daw::expecting( test1.value( ) == 1 );
	daw::expecting( test1.size( ) == 1 );
	daw::expecting( test1.pop_back( 1 ) == 1 );
	test1.clear( );
	daw::expecting( test1.value( ) == 0 );
	daw::expecting( test1.size( ) == 0 );

	{
		daw::basic_bit_queue<value_type, value_type> test2;
		test2.push_back( 2 );
		daw::expecting( test2.value( ) == 2 );
		daw::expecting( test2.size( ) == sizeof( value_type ) * 8 );
		auto result = test2.pop_back( 1 );
		daw::expecting( result == 0 );
	}

	uint16_t v = 1;
	test1.push_back( 1, 1 );

	for( size_t n = 1; n < sizeof( value_type ) * 8; ++n ) {
		test1.push_back( 0, 1 );
		v *= 2;
		daw::expecting( test1.value( ) == v );
	}

	for( size_t n = 1; n < sizeof( value_type ) * 8; ++n ) {
		int const result = test1.pop_back( 1 );
		daw::expecting( result == 0 );
	}
	daw::expecting( test1.value( ) == 1 );
	daw::expecting( test1.size( ) == 1 );
	daw::expecting( test1.pop_back( 1 ) == 1 );
	daw::expecting( test1.empty( ) );

	v = 1;
	test1.push_back( 1, 1 );
	for( size_t n = 1; n < sizeof( value_type ) * 8; ++n ) {
		test1.push_back( 0, 1 );
		v *= 2;
		daw::expecting( test1.value( ) == v );
	}
	daw::expecting( test1.pop_front( 1 ), 1 );
	for( size_t n = 1; n < sizeof( value_type ) * 8; ++n ) {
		int const result = test1.pop_front( 1 );
		daw::expecting( result == 0 );
	}
	daw::expecting( test1.size( ) == 0 );
	daw::expecting( test1.empty( ) );
	return true;
}

bool daw_bit_queues_test_002( ) {
	using value_type = uint8_t;
	daw::basic_bit_queue<value_type, value_type> test1;
	test1.push_back( std::numeric_limits<value_type>::max( ) );
	daw::expecting( test1.pop_back( 3 ) == 7 );
	return true;
}

bool daw_bit_queues_test_003( ) {
	using value_type = uint16_t;
	daw::basic_bit_queue<value_type, value_type> test1;
	test1.push_back( 37, 4 );
	daw::expecting( 5U, test1.value( ) );
	test1.pop_back( 1 );
	daw::expecting( 2U, test1.value( ) );
	return true;
}

void daw_nibble_queue_test_001( ) {
	using value_type = uint32_t;

	auto const nibble_to_hex = []( uint8_t c ) -> uint8_t {
		if( c < 10u ) {
			return '0' + c;
		} else if( c < 16u ) {
			return 'a' + ( c - 10u );
		} else {
			throw std::runtime_error( "Invalid nibble" );
		}
	};

	for( uint32_t n = 0; n < 32; ++n ) {
		daw::basic_nibble_queue<value_type, uint8_t> test1{ ( 1u << n ) };
		std::string str;
		while( test1.can_pop( 1 ) ) {
			str.push_back(
			  static_cast<char>( nibble_to_hex( test1.pop_front( 1 ) ) ) );
		}
		std::cout << str << std::endl;
	}
}

int main( ) {
	daw_bit_queues_test_001( );
	daw_bit_queues_test_002( );
	daw_bit_queues_test_003( );
	daw_nibble_queue_test_001( );
}
