// The MIT License (MIT)
//
// Copyright (c) 2014-2018 Darrell Wright
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

#include <iostream>
#include <sstream>
#include <string>

#include "daw/boost_test.h"
#include "daw/daw_traits.h"
#include "daw/daw_variant2.h"

struct test_t {
	std::string s;
	int foo;
	char a;
};

std::string to_string( test_t const &value ) {
	std::stringstream ss;
	ss << value.s << ": " << value.foo;
	return ss.str( );
}

BOOST_AUTO_TEST_CASE( daw_variant_001 ) {
	using namespace std::literals::string_literals;
	using c_t = char[41];
	using v_t = daw::variant<test_t, int, float, std::string, c_t>;
	std::cout << "size of variant is: " << sizeof( v_t ) << '\n';
	std::cout
	  << "size of largest type in variant is "
	  << daw::traits::max_sizeof_v<test_t, int, float, std::string, c_t> << '\n';
	std::cout << "size of type index is "
	          << sizeof( decltype( std::declval<v_t>( ).index( ) ) ) << '\n';
	v_t s{};
	v_t t = 5;
	std::string five = "5"s;
	v_t u = five;
	BOOST_REQUIRE_MESSAGE( s.empty( ), "Default value should be empty" );
	BOOST_REQUIRE_MESSAGE( t.get<int>( ) == 5, "Value not there" );
	BOOST_REQUIRE_MESSAGE( static_cast<int>( t ) == 5, "Value not there" );
	// TODO	BOOST_REQUIRE_MESSAGE( t == 5, "Value not there" );
	// TODO	BOOST_REQUIRE_MESSAGE( t.to_string( ) == "5", "too string not
	// functioning" );
	// TODO	BOOST_REQUIRE_MESSAGE( ( t == u ), "Type change" );
	// TODO	BOOST_REQUIRE_MESSAGE( *t == "5", "operator* conversion" );
	// TODO	BOOST_REQUIRE_MESSAGE( ( t = 5.54f ) == 5.54f, "Type change" );
	t = "5"s;
	t = test_t{};
	t = 5.5f;

	t = "hello"s;

	{
		v_t test_i{5};
		v_t test_s = std::string{"hello"};
		// TODO		BOOST_REQUIRE( test_i != test_s );
	}
}

struct blah {
	int a;
	int b;
};

std::string to_string( blah const &val ) {
	using std::to_string;
	return to_string( val.a ) + " " + to_string( val.b );
}

std::string to_string( std::vector<int> const &v ) {
	std::string result;
	for( auto i : v ) {
		result += std::to_string( i );
	}
	return result;
}

BOOST_AUTO_TEST_CASE( daw_variant_operators_001 ) {
	using var_t = daw::variant<int, blah>;

	var_t a = 5;
	var_t b = 6;
	var_t c = 6;
	var_t d = blah{};

	BOOST_REQUIRE( a != b );
	BOOST_REQUIRE( a != d );
	BOOST_REQUIRE( d != a );
	BOOST_REQUIRE( b == c );
	BOOST_REQUIRE( a < b );
	BOOST_REQUIRE( a <= b );
	BOOST_REQUIRE( c <= b );
	BOOST_REQUIRE( b > a );
	BOOST_REQUIRE( b >= c );

	using var_t2 = daw::variant<std::vector<int>, int, double>;
	var_t2 e = std::vector<int>{1, 2, 3};
	var_t2 f = std::vector<int>{1, 2, 3};
	var_t2 g = std::vector<int>{1};
	// TODO	BOOST_REQUIRE( e == f );
	// TODO	BOOST_REQUIRE( e != g );
}

namespace daw_variant_destructors_001_ns {

	struct temp_t {
		bool *b;

		constexpr temp_t( bool *B ) noexcept
		  : b{B} {}

		~temp_t( ) noexcept {
			if( auto tmp = std::exchange( b, nullptr ) ) {
				*tmp = true;
			}
		}
		constexpr temp_t( temp_t const &other ) noexcept
		  : b{other.b} {}

		constexpr temp_t( temp_t &&other ) noexcept
		  : b{other.b} {}

		constexpr temp_t &operator=( temp_t const &rhs ) noexcept {
			b = rhs.b;
			return *this;
		}

		constexpr temp_t &operator=( temp_t &&rhs ) noexcept {
			b = rhs.b;
			return *this;
		}
	};

	BOOST_AUTO_TEST_CASE( daw_variant_destructors_001 ) {
		bool b_test = false;
		daw::variant<temp_t, int> a = temp_t{&b_test};
		a = 5;
		BOOST_REQUIRE( b_test );
		// TODO		std::cout << to_string( a ) << '\n';
		a = temp_t{&b_test};
	}
} // namespace daw_variant_destructors_001_ns

BOOST_AUTO_TEST_CASE( daw_variant_recursive_001 ) {
	daw::variant<int, daw::variant<int, bool>> a{1};
	// TODO	BOOST_REQUIRE( a == 1 );
	a = daw::variant<int, bool>{false};
	// TODO	BOOST_REQUIRE( a != 1 );
}

BOOST_AUTO_TEST_CASE( daw_variant_swap_001 ) {
	daw::variant<int> a{1};
	daw::variant<int> b{2};
	using std::swap;
	swap( a, b );
	// TODO	BOOST_REQUIRE( a == 2 );
}

namespace daw_variant_optional_001_ns {
	template<typename T>
	using optional = daw::variant<T>;

	BOOST_AUTO_TEST_CASE( daw_variant_optional_001 ) {
		optional<int> a{5};
		optional<int> b;

		BOOST_REQUIRE( a );
		BOOST_REQUIRE( b.empty( ) );
		// TODO			BOOST_REQUIRE( a != b );
		a.reset( );
		// TODO			BOOST_REQUIRE( a == b );
	}
} // namespace daw_variant_optional_001_ns
