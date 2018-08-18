// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdexcept>

#include "daw/boost_test.h"
#include "daw/daw_validated.h"

template<typename T, T min_value, T max_value>
struct int_validator_t {
	template<typename U>
	constexpr bool operator( )( U &&value ) const noexcept {
		return value >= min_value && value <= max_value;
	}
};

BOOST_AUTO_TEST_CASE( int_range_test_good_001 ) {
	using value_t = daw::validated<int, int_validator_t<int, 0, 100>>;
	constexpr value_t tmp( 0 );
	BOOST_REQUIRE_EQUAL( tmp, 0 );

	constexpr value_t tmp2( 100 );
	BOOST_REQUIRE_EQUAL( tmp2, 100 );

	constexpr int tmp3 = value_t( 0 );
	BOOST_REQUIRE_EQUAL( tmp3, 0 );

	value_t tmp4 = 5;
	BOOST_REQUIRE_EQUAL( tmp4, 5 );
	tmp4 = 100;
	int tmp5 = 40;
	tmp4 = std::move( tmp5 );
}

template<typename T, typename U>
constexpr void assign( T & lhs, U && rhs ) {
	lhs = std::forward<U>( rhs );
}

BOOST_AUTO_TEST_CASE( int_range_test_bad_001 ) {
	using value_t = daw::validated<int, int_validator_t<int, 0, 100>>;
	BOOST_REQUIRE_THROW( value_t( -1 ), std::out_of_range );
	BOOST_REQUIRE_THROW( value_t( 101 ), std::out_of_range );
	value_t tmp = 1;
	BOOST_REQUIRE_EQUAL( tmp, 1 );
	BOOST_REQUIRE_THROW( assign( tmp, 101 ), std::out_of_range );
	BOOST_REQUIRE_EQUAL( tmp, 1 );
	int tmp2 = 101; 
	BOOST_REQUIRE_THROW( assign( tmp, std::move( tmp2 ) ), std::out_of_range );
	BOOST_REQUIRE_EQUAL( tmp, 1 );

}

enum class enum_t { apple = 0, orange = 1, bannana = 3 };

struct enum_validator_t {
	template<typename T>
	constexpr bool operator( )( T const &value ) const noexcept {
		using u_t = std::underlying_type_t<enum_t>;
		switch( static_cast<u_t>( value ) ) {
		case static_cast<u_t>( enum_t::apple ):
		case static_cast<u_t>( enum_t::orange ):
		case static_cast<u_t>( enum_t::bannana ):
			return true;
		default:
			return false;
		}
	}
};

BOOST_AUTO_TEST_CASE( enum_test_good_001 ) {
	using value_t = daw::validated<enum_t, enum_validator_t>;
	constexpr auto tmp = value_t( 1 );
	BOOST_REQUIRE( tmp == enum_t::orange );
	BOOST_REQUIRE( value_t( enum_t::apple ) == enum_t::apple );
	BOOST_REQUIRE( value_t( ) == enum_t::apple );
}

BOOST_AUTO_TEST_CASE( enum_test_bad_001 ) {
	using value_t = daw::validated<enum_t, enum_validator_t>;
	BOOST_REQUIRE_THROW( value_t( 5 ), std::out_of_range );
	BOOST_REQUIRE_THROW( value_t( -1 ), std::out_of_range );
}
