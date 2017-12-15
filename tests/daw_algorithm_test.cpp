// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "boost_test.h"
#include "daw_algorithm.h"

BOOST_AUTO_TEST_CASE( daw_transform_many ) {
	std::vector<uint32_t> in1 = {1, 3, 5, 7, 9};
	std::vector<uint32_t> in2 = {0, 2, 4, 6, 8};
	std::vector<uint32_t> out;

	daw::algorithm::transform_many( in1.begin( ), in1.end( ), in2.begin( ), std::back_inserter( out ),
	                                []( auto const &v1, auto const &v2 ) { return v1 + v2; } );
	BOOST_REQUIRE_MESSAGE( out.size( ) == in1.size( ), "1. Incorrect size on output" );
}

constexpr bool quick_sort_test( ) noexcept {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::quick_sort( blah, blah + 6 );
	return daw::algorithm::is_sorted( blah, blah + 6 );
}

constexpr bool sort_test( ) noexcept {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::insertion_sort( blah, blah + 6 );
	return daw::algorithm::is_sorted( blah, blah + 6 );
}

BOOST_AUTO_TEST_CASE( daw_sort_test_001 ) {
	constexpr bool sort_tst = sort_test( );
	BOOST_REQUIRE( sort_tst );
}

BOOST_AUTO_TEST_CASE( daw_sort_test_002 ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::insertion_sort( blah, blah + 6 );
	for( auto i : blah ) {
		std::cout << i << " ";
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_sort_test_003 ) {
	constexpr bool sort_tst = quick_sort_test( );
	BOOST_REQUIRE( sort_tst );
}

BOOST_AUTO_TEST_CASE( daw_sort_test_004 ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::quick_sort( blah, blah + 6 );
	for( auto i : blah ) {
		std::cout << i << " ";
	}
	std::cout << '\n';
}

BOOST_AUTO_TEST_CASE( daw_map_test_001 ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::map( blah, daw::algorithm::next( blah, 6 ), blah, []( auto val ) { return val % 2 == 0 ? 0 : 1; } );
}

constexpr int map_test_002( ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	struct {
		constexpr int operator( )( int val ) const noexcept {
			return val % 2 == 0 ? 0 : 1;
		}
	} unary_op{};

	daw::algorithm::map( blah, daw::algorithm::next( blah, 6 ), blah, unary_op );
	int result = 0;
	for( size_t n = 0; n < 6; ++n ) {
		result += blah[n];
	}
	return result;
}

BOOST_AUTO_TEST_CASE( daw_map_test_002 ) {
	constexpr auto const tst = map_test_002( );
	BOOST_REQUIRE_EQUAL( tst, 3 );
}

BOOST_AUTO_TEST_CASE( daw_reduce_test_001 ) {
	int blah[6] = {1, 0, 1, 0, 1, 0};
	auto const tst = daw::algorithm::reduce( blah, daw::algorithm::next( blah, 6 ),
	                                         0, []( auto lhs, auto rhs ) noexcept { return lhs + rhs; } );
	BOOST_REQUIRE_EQUAL( tst, 3 );
}

namespace copy_n_ns {
	constexpr bool test_copy_n( ) {
		char const src[] = "This is a test.";
		char dst[16] = {0};
		daw::algorithm::copy_n( src, dst, 16 );
		return true;
	}
	BOOST_AUTO_TEST_CASE( daw_copy_n_test_001 ) {
		constexpr auto b = test_copy_n( );
	}
} // namespace copy_n_ns

BOOST_AUTO_TEST_CASE( daw_transform_it_test_001 ) {
	std::vector<int> a = {1, 2, 3, 4, 5, 6};
	std::vector<int> b{};
	daw::algorithm::transform_it( a.cbegin( ), a.cend( ), std::back_inserter( b ), []( int a_val, auto out_it ) {
		for( size_t n = 0; static_cast<int>( n ) < a_val; ++n ) {
			*out_it++ = a_val;
		}
		return out_it;
	} );
}

BOOST_AUTO_TEST_CASE( daw_minmax_element_test_001 ) {
	std::vector<int> tst = {1, 3, 2, -1, 100, -50, 1, 5, 2442};
	auto result = daw::algorithm::minmax_element( tst.cbegin( ), tst.cend( ) );

	BOOST_REQUIRE_EQUAL( *( result.min_element ), -50 );
	BOOST_REQUIRE_EQUAL( *( result.max_element ), 2442 );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_one_test_001 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst, []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 11 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 12 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 13 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 5 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 15 ) != v.cend( ); } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}
BOOST_AUTO_TEST_CASE( daw_satisfies_one_test_002 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst, []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 11 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 12 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 13 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 5 ) != v.cend( ); },
	  []( auto const &v ) { return std::find( v.cbegin( ), v.cend( ), 15 ) != v.cend( ); } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, false );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_one_test_003 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one( tst.begin( ), tst.end( ), []( auto v ) { return v == 11; },
	                                                []( auto v ) { return v == 12; }, []( auto v ) { return v == 13; },
	                                                []( auto v ) { return v == 14; }, []( auto v ) { return v == 15; } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, false );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_one_test_004 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one( tst.begin( ), tst.end( ), []( auto v ) { return v == 11; },
	                                                []( auto v ) { return v == 12; }, []( auto v ) { return v == 13; },
	                                                []( auto v ) { return v == 5; }, []( auto v ) { return v == 15; } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_001 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all( tst, []( auto const &v ) { return v.size( ) == 11; },
	                                                []( auto const &v ) { return v.front( ) == 0; },
	                                                []( auto const &v ) { return v.back( ) == 10; } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}
BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_002 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all( tst, []( auto const &v ) { return v.size( ) == 11; },
	                                                []( auto const &v ) { return v.front( ) == 0; },
	                                                []( auto const &v ) { return v.back( ) == 11; } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, false );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_003 ) {
	std::vector<int> const tst = {0, 2, 4, 6, 8, 10};

	auto const ans =
	  daw::algorithm::satisfies_all( tst.begin( ), tst.end( ), []( auto v ) { return v < 11; },
	                                 []( auto v ) { return v >= 0; }, []( auto v ) { return v % 2 == 0; } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_004 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans =
	  daw::algorithm::satisfies_all( tst.begin( ), tst.end( ), []( auto v ) { return v < 11; },
	                                 []( auto v ) { return v >= 0; }, []( auto v ) { return v % 2 == 0; } );

	constexpr bool is_correct_type = daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, false );
}

BOOST_AUTO_TEST_CASE( daw_in_range_test_001 ) {
	auto tst = daw::algorithm::in_range( 1, 5 );

	BOOST_REQUIRE( tst( 4 ) );
	BOOST_REQUIRE( tst( 1 ) );
	BOOST_REQUIRE( tst( 5 ) );
	BOOST_REQUIRE( !tst( 0 ) );
	BOOST_REQUIRE( !tst( 6 ) );
	BOOST_REQUIRE( !tst( 14 ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_to_test_001 ) {
	auto tst = daw::algorithm::equal_to( 5 );

	BOOST_REQUIRE( tst( 5 ) );
	BOOST_REQUIRE( !tst( -1 ) );
}

BOOST_AUTO_TEST_CASE( daw_greater_than_test_001 ) {
	auto tst = daw::algorithm::greater_than( 5 );

	BOOST_REQUIRE( tst( 6 ) );
	BOOST_REQUIRE( !tst( 5 ) );
	BOOST_REQUIRE( !tst( -1 ) );
}

BOOST_AUTO_TEST_CASE( daw_greater_than_or_equal_totest_001 ) {
	auto tst = daw::algorithm::greater_than_or_equal_to( 5 );

	BOOST_REQUIRE( tst( 6 ) );
	BOOST_REQUIRE( tst( 5 ) );
	BOOST_REQUIRE( !tst( -1 ) );
}

BOOST_AUTO_TEST_CASE( daw_less_than_test_001 ) {
	auto tst = daw::algorithm::less_than( 5 );

	BOOST_REQUIRE( !tst( 6 ) );
	BOOST_REQUIRE( !tst( 5 ) );
	BOOST_REQUIRE( tst( -1 ) );
}

BOOST_AUTO_TEST_CASE( daw_less_than_or_equal_totest_001 ) {
	auto tst = daw::algorithm::less_than_or_equal_to( 5 );

	BOOST_REQUIRE( !tst( 6 ) );
	BOOST_REQUIRE( tst( 5 ) );
	BOOST_REQUIRE( tst( -1 ) );
}
