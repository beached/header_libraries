// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include <array>
#include <memory>
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
	static_assert( value_t( 0 ) == 0, "" );
	static_assert( value_t( 100 ) == 100, "" );
	static_assert( value_t( 0 ) == 0, "" );

	value_t tmp4 = 5;
	BOOST_REQUIRE_EQUAL( tmp4, 5 );
	tmp4 = 100;
	int tmp5 = 40;
	tmp4 = std::move( tmp5 );
}

template<typename T, typename U>
constexpr void assign( T &lhs, U &&rhs ) {
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

struct no_repeat_container {
	template<typename Container>
	bool operator( )( Container const &c ) noexcept {
		auto const last = c.end( );
		for( auto it = c.begin( ); it != last; ++it ) {
			if( std::find( std::next( it ), last, *it ) != last ) {
				return false;
			}
		}
		return true;
	}
};

BOOST_AUTO_TEST_CASE( array_good_001 ) {
	using value_t = daw::validated<std::array<int, 5>, no_repeat_container>;
	value_t tmp( 0, 1, 2, 3, 4 );
	BOOST_REQUIRE_EQUAL( tmp.get( ).size( ), 5 );

	std::array<int, 5> tmp2 = value_t( 0, 1, 2, 3, 4 );
	BOOST_REQUIRE_EQUAL( tmp2[0], 0 );
	BOOST_REQUIRE_EQUAL( tmp2[1], 1 );
	BOOST_REQUIRE_EQUAL( tmp2[2], 2 );
	BOOST_REQUIRE_EQUAL( tmp2[3], 3 );
	BOOST_REQUIRE_EQUAL( tmp2[4], 4 );
} // namespace array_good_001

BOOST_AUTO_TEST_CASE( array_bad_001 ) {
	using value_t = daw::validated<std::array<int, 5>, no_repeat_container>;
	BOOST_REQUIRE_THROW( value_t( 1, 1, 2, 3, 4 ), std::out_of_range );
}

struct test_class_t {
	int value;
	constexpr int calc( int n ) const {
		return value * n;
	}

	// Should never get called via validated
	constexpr int calc( int n ) {
		return value + n;
	}
};

struct test_class_validator_t {
	constexpr bool operator( )( test_class_t const &c ) noexcept {
		return c.value % 2 == 0;
	}
};

namespace struct_good_001 {
	using value_t = daw::validated<test_class_t, test_class_validator_t>;
	static constexpr value_t a = {2};
	static_assert( a.get( ).value == 2, "" );
} // namespace struct_good_001

BOOST_AUTO_TEST_CASE( struct_bad_001 ) {
	using value_t = daw::validated<test_class_t, test_class_validator_t>;
	BOOST_REQUIRE_THROW( value_t( 1 ), std::out_of_range );
}

struct test_class2_validator_t {
	constexpr bool operator( )( std::unique_ptr<int> &c ) noexcept {
		return true;
	}
};

BOOST_AUTO_TEST_CASE( struct_move_001 ) {
	using value_t = daw::validated<std::unique_ptr<int>, test_class2_validator_t>;
	auto a = value_t( std::make_unique<int>( 2 ) );

	std::unique_ptr<int> b( std::move( a ).get( ) );

	BOOST_REQUIRE( !a.get( ) );

	BOOST_REQUIRE_EQUAL( *b, 2 );
}

BOOST_AUTO_TEST_CASE( operator_star_001 ) {
	using value_t = daw::validated<test_class_t, test_class_validator_t>;

	constexpr value_t a = {2};
	static_assert( a.get( ).value == 2, "" );

	static_assert( ( *a ).calc( 2 ) == 4, "" );
	test_class_t a_tmp = a;

	BOOST_REQUIRE( ( *a ).calc( 5 ) != a_tmp.calc( 5 ) );

	constexpr value_t const b = {4};
	static_assert( b.get( ).value == 4, "" );
	static_assert( ( *b ).calc( 2 ) == 8, "" );

	static_assert( ( *value_t( 2 ) ).calc( 2 ) == 4, "" );
} // namespace operator_star_001

namespace operator_right_arrow_001 {
	using value_t = daw::validated<test_class_t, test_class_validator_t>;

	static constexpr value_t a = {2};
	static_assert( a.get( ).value == 2, "" );

	static_assert( a->calc( 2 ) == 4, "" );

	static constexpr value_t const b = {4};
	static_assert( b.get( ).value == 4, "" );
	static_assert( b->calc( 2 ) == 8, "" );

	static_assert( value_t( 2 )->calc( 2 ) == 4, "" );
} // namespace operator_right_arrow_001

constexpr int
function_argument_test( daw::validated<int, int_validator_t<int, 5, 10>> arg ) {
	return arg * 2;
}

BOOST_AUTO_TEST_CASE( function_argument_001 ) {
	static_assert( function_argument_test( 6 ) == 12, "" );
	BOOST_REQUIRE_THROW( function_argument_test( 15 ), std::out_of_range );
}

struct throwing_validator {
	constexpr bool operator( )( int const &v ) {
		if( v % 2 == 0 ) {
			throw std::runtime_error( "V must be odd" );
		}
		return true;
	}
};

BOOST_AUTO_TEST_CASE( throwing_validator_001 ) {
	using value_t = daw::validated<int, throwing_validator>;
	static_assert( value_t( 1 ) == 1, "" );

	BOOST_REQUIRE_THROW( value_t( 2 ), std::runtime_error );
}
