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

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "daw/boost_test.h"
#include "daw/daw_algorithm.h"

BOOST_AUTO_TEST_CASE( daw_safe_advance_test_001 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, 5 );
	bool const ans = ( it != std::begin( a ) );
	BOOST_REQUIRE( ans );
	auto it2 = std::next( std::begin( a ), 5 );
	bool const ans2 = it == it2;
	BOOST_REQUIRE( ans2 );
}

BOOST_AUTO_TEST_CASE( daw_safe_advance_test_002 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, static_cast<ptrdiff_t>( a.size( ) + 5u ) );
	bool const ans = ( it == std::end( a ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_advance_test_003 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, -5 );
	bool const ans = ( it == std::begin( a ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_advance_test_004 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, 5 );
	bool const ans = ( it != std::begin( a ) );
	BOOST_REQUIRE( ans );
	auto it2 = std::next( std::begin( a ), 5 );
	bool const ans2 = it == it2;
	BOOST_REQUIRE( ans2 );
}

BOOST_AUTO_TEST_CASE( daw_safe_advance_test_005 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, static_cast<ptrdiff_t>( a.size( ) + 5u ) );
	bool const ans = ( it == std::end( a ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_advance_test_006 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, -5 );
	bool const ans = ( it == std::begin( a ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_next_test_001 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_next" );

	bool const ans = it == std::next( std::begin( a ), 5 );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_next_test_002 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), a.size( ) + 5 );

	bool const ans = it == std::end( a );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_next_test_003 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_next" );

	bool const ans = it == std::next( std::begin( a ), 5 );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_next_test_004 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), a.size( ) + 5 );

	bool const ans = it == std::end( a );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_prev_test_001 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( std::end( a ), std::begin( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_prev" );

	bool const ans = it == std::prev( std::end( a ), 5 );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_prev_test_002 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( std::end( a ), std::begin( a ), a.size( ) + 5 );

	bool const ans = it == std::begin( a );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_prev_test_003 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( std::end( a ), std::begin( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_prev" );

	bool const ans = it == std::prev( std::end( a ), 5 );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_safe_prev_test_004 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( std::end( a ), std::begin( a ), a.size( ) + 5 );

	bool const ans = it == std::begin( a );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_begin_at_test_001 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 0 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in begin_at" );

	bool const ans = it == std::begin( a );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_begin_at_test_002 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 5 );
	bool const ans = it == std::next( std::begin( a ), 5 );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_begin_at_test_003 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 0 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in begin_at" );

	bool const ans = it == std::begin( a );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_begin_at_test_004 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 5 );
	bool const ans = it == std::next( std::begin( a ), 5 );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_transform_many ) {
	std::vector<uint32_t> in1 = {1, 3, 5, 7, 9};
	std::vector<uint32_t> in2 = {0, 2, 4, 6, 8};
	std::vector<uint32_t> out;

	daw::algorithm::transform_many(
	  in1.begin( ), in1.end( ), in2.begin( ), std::back_inserter( out ),
	  []( auto const &v1, auto const &v2 ) { return v1 + v2; } );
	BOOST_REQUIRE_MESSAGE( out.size( ) == in1.size( ),
	                       "1. Incorrect size on output" );
}

/* TODO: remove
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
*/

BOOST_AUTO_TEST_CASE( daw_map_test_001 ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::map( blah, daw::next( blah, 6 ), blah,
	                     []( auto val ) { return val % 2 == 0 ? 0 : 1; } );
}

constexpr int map_test_002( ) {
	int blah[6] = {23, 5, 2, -1, 100, -1000};
	struct {
		constexpr int operator( )( int val ) const noexcept {
			return val % 2 == 0 ? 0 : 1;
		}
	} unary_op{};

	daw::algorithm::map( blah, daw::next( blah, 6 ), blah, unary_op );
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
	auto const tst = daw::algorithm::reduce( blah, daw::next( blah, 6 ), 0, [
	]( auto lhs, auto rhs ) noexcept { return lhs + rhs; } );
	BOOST_REQUIRE_EQUAL( tst, 3 );
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
	  tst,
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 11 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 12 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 13 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 5 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 15 ) != v.cend( );
	  } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_one_test_002 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst,
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 11 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 12 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 13 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 5 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return std::find( v.cbegin( ), v.cend( ), 15 ) != v.cend( );
	  } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, false );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_one_test_003 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst.begin( ), tst.end( ), []( auto v ) { return v == 11; },
	  []( auto v ) { return v == 12; }, []( auto v ) { return v == 13; },
	  []( auto v ) { return v == 14; }, []( auto v ) { return v == 15; } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, false );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_one_test_004 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst.begin( ), tst.end( ), []( auto v ) { return v == 11; },
	  []( auto v ) { return v == 12; }, []( auto v ) { return v == 13; },
	  []( auto v ) { return v == 5; }, []( auto v ) { return v == 15; } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_001 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst, []( auto const &v ) { return v.size( ) == 11; },
	  []( auto const &v ) { return v.front( ) == 0; },
	  []( auto const &v ) { return v.back( ) == 10; } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_002 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst, []( auto const &v ) { return v.size( ) == 11; },
	  []( auto const &v ) { return v.front( ) == 0; },
	  []( auto const &v ) { return v.back( ) == 11; } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, false );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_003 ) {
	std::vector<int> const tst = {0, 2, 4, 6, 8, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst.begin( ), tst.end( ), []( auto v ) { return v < 11; },
	  []( auto v ) { return v >= 0; }, []( auto v ) { return v % 2 == 0; } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
	BOOST_REQUIRE_EQUAL( is_correct_type, true );
	BOOST_REQUIRE_EQUAL( ans, true );
}

BOOST_AUTO_TEST_CASE( daw_satisfies_all_test_004 ) {
	std::vector<int> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst.begin( ), tst.end( ), []( auto v ) { return v < 11; },
	  []( auto v ) { return v >= 0; }, []( auto v ) { return v % 2 == 0; } );

	constexpr bool is_correct_type =
	  daw::is_same_v<bool, std::decay_t<decltype( ans )>>;
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

BOOST_AUTO_TEST_CASE( daw_greater_than_or_equal_to_test_001 ) {
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

BOOST_AUTO_TEST_CASE( daw_less_than_or_equal_to_test_001 ) {
	auto tst = daw::algorithm::less_than_or_equal_to( 5 );

	BOOST_REQUIRE( !tst( 6 ) );
	BOOST_REQUIRE( tst( 5 ) );
	BOOST_REQUIRE( tst( -1 ) );
}

BOOST_AUTO_TEST_CASE( daw_lexigraphical_compare_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto ans = daw::algorithm::lexicographical_compare( a.cbegin( ), a.cend( ),
	                                                    b.cbegin( ), b.cend( ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_lexigraphical_compare_test_002 ) {
	std::vector<int> const a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto ans = daw::algorithm::lexicographical_compare( a.cbegin( ), a.cend( ),
	                                                    b.cbegin( ), b.cend( ) );
	BOOST_REQUIRE( !ans );
}

BOOST_AUTO_TEST_CASE( daw_transform_if_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> b{};
	std::vector<int> const expected_b = {-1, 1, 3, 5, 7, 9};

	daw::algorithm::transform_if( a.cbegin( ), a.cend( ), std::back_inserter( b ),
	                              []( auto const &v ) { return v % 2 == 0; },
	                              []( auto const &v ) { return v - 1; } );

	auto ans = std::equal( b.cbegin( ), b.cend( ), expected_b.cbegin( ),
	                       expected_b.cend( ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_transform_n_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> b{};
	std::vector<int> const expected_b = {0, 2, 4, 6};

	daw::algorithm::transform_n( a.cbegin( ), std::back_inserter( b ), 4,
	                             []( auto const &v ) { return v * 2; } );

	auto ans = std::equal( b.cbegin( ), b.cend( ), expected_b.cbegin( ),
	                       expected_b.cend( ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_transform_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> b{};
	std::vector<int> const expected_b = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

	daw::algorithm::transform( a.cbegin( ), a.cend( ), std::back_inserter( b ),
	                           []( auto const &v ) { return v * 2; } );

	auto ans = std::equal( b.cbegin( ), b.cend( ), expected_b.cbegin( ),
	                       expected_b.cend( ) );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_transform_it_test_001 ) {
	std::vector<int> a = {1, 2, 3, 4, 5, 6};
	std::vector<int> b{};
	daw::algorithm::transform_it( a.cbegin( ), a.cend( ), std::back_inserter( b ),
	                              []( int a_val, auto out_it ) {
		                              for( size_t n = 0;
		                                   static_cast<int>( n ) < a_val; ++n ) {
			                              *out_it++ = a_val;
		                              }
		                              return out_it;
	                              } );
}

BOOST_AUTO_TEST_CASE( daw_copy_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> b{};

	daw::algorithm::copy( a.cbegin( ), a.cend( ), std::back_inserter( b ) );
	BOOST_REQUIRE( std::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_copy_n_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> b{};
	std::vector<int> const expected_b = {0, 1, 2, 3, 4, 5};

	daw::algorithm::copy_n( a.cbegin( ), std::back_inserter( b ), 6 );
	BOOST_REQUIRE( std::equal( b.cbegin( ), b.cend( ), expected_b.cbegin( ),
	                           expected_b.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_move_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto expected_b = a;
	std::vector<int> b{};

	daw::algorithm::move( a.cbegin( ), a.cend( ), std::back_inserter( b ) );
	BOOST_REQUIRE( std::equal( expected_b.cbegin( ), expected_b.cend( ),
	                           b.cbegin( ), b.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_move_n_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> b{};
	std::vector<int> const expected_b = {0, 1, 2, 3, 4, 5};

	daw::algorithm::move_n( a.cbegin( ), std::back_inserter( b ), 6 );
	BOOST_REQUIRE( std::equal( b.cbegin( ), b.cend( ), expected_b.cbegin( ),
	                           expected_b.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto const b = a;
	BOOST_REQUIRE( daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_002 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {0, 1, 2, 3, 4, 1, 6, 7, 8, 9, 10};
	BOOST_REQUIRE(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_003 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto const b = a;
	BOOST_REQUIRE(
	  daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_004 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {0, 1, 2, 3, 4, 1, 6, 7, 8, 9, 10};
	BOOST_REQUIRE(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_005 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {0, 1, 2, 3, 4};
	BOOST_REQUIRE(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_006 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	BOOST_REQUIRE(
	  daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ),
	                         []( auto lhs, auto rhs ) { return lhs == rhs; } ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_007 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {0, 1, 2, 5, 4, 5, 6, 7, 8, 9, 10};
	BOOST_REQUIRE(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ),
	                          []( auto lhs, auto rhs ) { return lhs == rhs; } ) );
}

BOOST_AUTO_TEST_CASE( daw_equal_test_008 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> const b = {0, 1, 3, 4, 5, 6, 7, 8, 9, 10};
	BOOST_REQUIRE(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ),
	                          []( auto lhs, auto rhs ) { return lhs == rhs; } ) );
}

BOOST_AUTO_TEST_CASE( daw_swapper_test_001 ) {
	std::string a{"a"};
	std::string b{"b"};
	daw::algorithm::swapper( a, b );
	BOOST_REQUIRE_EQUAL( a, "b" );
	BOOST_REQUIRE_EQUAL( b, "a" );
}

BOOST_AUTO_TEST_CASE( daw_rotate_test_001 ) {
	std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	daw::algorithm::rotate( a.begin( ), std::next( a.begin( ), 5 ), a.end( ) );
	BOOST_REQUIRE_EQUAL( a.front( ), 5 );
	BOOST_REQUIRE_EQUAL( a.back( ), 4 );
	BOOST_REQUIRE_EQUAL( a[5], 10 );
}

BOOST_AUTO_TEST_CASE( daw_upper_bound_test_001 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto tst = daw::algorithm::upper_bound( a.cbegin( ), a.cend( ), 5 );
	auto ans = tst == std::next( a.cbegin( ), 6 );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_upper_bound_test_002 ) {
	std::vector<int> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto tst = daw::algorithm::upper_bound( a.cbegin( ), a.cend( ), 20 );
	auto ans = tst == a.cend( );
	BOOST_REQUIRE( ans );
}

BOOST_AUTO_TEST_CASE( daw_minmax_item_test_001 ) {
	int a = 5;
	int b = -100;
	auto ans = daw::algorithm::minmax_item( a, b );
	BOOST_REQUIRE_EQUAL( ans.first, -100 );
	BOOST_REQUIRE_EQUAL( ans.second, 5 );
}

BOOST_AUTO_TEST_CASE( daw_minmax_item_test_002 ) {
	struct A {
		int v;
	};
	A a{5};
	A b{-100};
	auto ans = daw::algorithm::minmax_item(
	  a, b, []( auto const &lhs, auto const &rhs ) { return lhs.v < rhs.v; } );
	BOOST_REQUIRE_EQUAL( ans.first.v, -100 );
	BOOST_REQUIRE_EQUAL( ans.second.v, 5 );
}
