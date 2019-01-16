// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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
#include <cstdint>
#include <iterator>
#include <string>
#include <unordered_map>

#include "daw/daw_algorithm.h"
#include "daw/daw_benchmark.h"

constexpr bool daw_safe_advance_test_001( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, 5 );
	bool const ans = ( it != std::begin( a ) );
	daw::expecting( ans );
	auto it2 = std::next( std::begin( a ), 5 );
	bool const ans2 = it == it2;
	daw::expecting( ans2 );
	return true;
}
static_assert( daw_safe_advance_test_001( ) );

constexpr bool binary_search_001( ) noexcept {
	int arry[10] = {1, 5, 10, 15, 16, 17, 18, 19, 20, 21};
	auto pos = daw::algorithm::binary_search( arry, arry + 10, 17 );

	return *pos == 17;
}
static_assert( binary_search_001( ), "" );

constexpr bool lower_bound_001( ) noexcept {
	std::array<int, 10> arry = {1, 5, 10, 15, 16, 17, 18, 19, 20, 21};
	auto pos =
	  daw::algorithm::lower_bound( std::begin( arry ), std::end( arry ), 17 );
	return *pos == 17;
}
static_assert( lower_bound_001( ), "" );

constexpr bool daw_safe_advance_test_002( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, static_cast<ptrdiff_t>( a.size( ) + 5u ) );
	bool const ans = ( it == std::end( a ) );
	daw::expecting( ans );
	return true;
}
static_assert( daw_safe_advance_test_002( ) );

constexpr bool daw_safe_advance_test_003( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, -5 );
	bool const ans = ( it == std::begin( a ) );
	daw::expecting( ans );
	return true;
}
static_assert( daw_safe_advance_test_003( ) );

constexpr bool daw_safe_advance_test_004( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, 5 );
	bool const ans = ( it != std::begin( a ) );
	daw::expecting( ans );
	auto it2 = std::next( std::begin( a ), 5 );
	bool const ans2 = it == it2;
	daw::expecting( ans2 );
	return true;
}
static_assert( daw_safe_advance_test_004( ) );

constexpr bool daw_safe_advance_test_005( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, static_cast<ptrdiff_t>( a.size( ) + 5u ) );
	bool const ans = ( it == std::end( a ) );
	daw::expecting( ans );
	return true;
}
static_assert( daw_safe_advance_test_005( ) );

constexpr bool daw_safe_advance_test_006( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = std::begin( a );
	daw::safe_advance( a, it, -5 );
	bool const ans = ( it == std::begin( a ) );
	daw::expecting( ans );
	return true;
}
static_assert( daw_safe_advance_test_006( ) );

constexpr bool daw_safe_next_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_next" );

	daw::expecting( it, std::next( std::begin( a ), 5 ) );
	return true;
}
static_assert( daw_safe_next_test_001( ) );

constexpr bool daw_safe_next_test_002( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), a.size( ) + 5 );

	daw::expecting( it, std::end( a ) );
	return true;
}
static_assert( daw_safe_next_test_002( ) );

constexpr bool daw_safe_next_test_003( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_next" );

	daw::expecting( it, std::next( std::begin( a ), 5 ) );
	return true;
}
static_assert( daw_safe_next_test_003( ) );

constexpr bool daw_safe_next_test_004( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_next( std::begin( a ), std::end( a ), a.size( ) + 5 );

	daw::expecting( it, std::end( a ) );
	return true;
}
static_assert( daw_safe_next_test_004( ) );

constexpr bool daw_safe_prev_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( std::end( a ), std::begin( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_prev" );

	daw::expecting( it, std::prev( std::end( a ), 5 ) );
	return true;
}
static_assert( daw_safe_prev_test_001( ) );

constexpr bool daw_safe_prev_test_002( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( a.data( ) + a.size( ), a.data( ), a.size( ) + 5 );

	daw::expecting( it, a.data( ) );
	return true;
}
static_assert( daw_safe_prev_test_002( ) );

constexpr bool daw_safe_prev_test_003( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( std::end( a ), std::begin( a ), 5 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in safe_prev" );

	daw::expecting( it, std::prev( std::end( a ), 5 ) );
	return true;
}
static_assert( daw_safe_prev_test_003( ) );

constexpr bool daw_safe_prev_test_004( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::safe_prev( std::end( a ), std::begin( a ), a.size( ) + 5 );

	daw::expecting( it, std::begin( a ) );
	return true;
}
static_assert( daw_safe_prev_test_004( ) );

constexpr bool daw_begin_at_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 0 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in begin_at" );

	daw::expecting( it, std::begin( a ) );
	return true;
}
static_assert( daw_begin_at_test_001( ) );

constexpr bool daw_begin_at_test_002( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 5 );
	daw::expecting( it, std::next( std::begin( a ), 5 ) );
	return true;
}
static_assert( daw_begin_at_test_002( ) );

constexpr bool daw_begin_at_test_003( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 0 );

	static_assert( daw::is_same_v<decltype( it ), decltype( std::begin( a ) )>,
	               "Iterator type is changing in begin_at" );

	daw::expecting( it, std::begin( a ) );
	return true;
}
static_assert( daw_begin_at_test_003( ) );

constexpr bool daw_begin_at_test_004( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto it = daw::begin_at( a, 5 );
	daw::expecting( it, std::next( std::begin( a ), 5 ) );
	return true;
}
static_assert( daw_begin_at_test_004( ) );

constexpr bool daw_transform_many( ) {
	std::array<uint32_t, 5> in1 = {1, 3, 5, 7, 9};
	std::array<uint32_t, 5> in2 = {0, 2, 4, 6, 8};
	std::array<uint32_t, 5> out{};

	daw::algorithm::transform_many(
	  begin( in1 ), end( in1 ), begin( in2 ), out.data( ),
	  []( auto const &v1, auto const &v2 ) { return v1 + v2; } );

	daw::expecting_message( out.size( ) == in1.size( ),
	                        "1. Incorrect size on output" );
	return true;
}
static_assert( daw_transform_many( ) );

constexpr bool daw_map_test_001( ) {
	std::array<int, 6> blah = {23, 5, 2, -1, 100, -1000};
	daw::algorithm::map( blah.cbegin( ), blah.cend( ), blah.begin( ),
	                     []( auto val ) { return val % 2 == 0 ? 0 : 1; } );

	int sum = 0;
	for( auto v : blah ) {
		sum += v;
	}
	daw::expecting( 3, sum );
	return true;
}
static_assert( daw_map_test_001( ) );

constexpr bool daw_map_test_002( ) {
	std::array<int, 6> blah = {23, 5, 2, -1, 100, -1000};
	struct {
		constexpr int operator( )( int val ) const noexcept {
			return val % 2 == 0 ? 0 : 1;
		}
	} unary_op{};
	daw::algorithm::map( blah.begin( ), blah.end( ), blah.begin( ), unary_op );
	int result = 0;
	for( size_t n = 0; n < 6; ++n ) {
		result += blah[n];
	}
	daw::expecting( 3, result );
	return true;
}
static_assert( daw_map_test_002( ) );

constexpr bool daw_reduce_test_001( ) {
	int blah[6] = {1, 0, 1, 0, 1, 0};
	auto const tst = daw::algorithm::reduce( blah, daw::next( blah, 6 ), 0, [
	]( auto lhs, auto rhs ) noexcept { return lhs + rhs; } );
	daw::expecting( 3, tst );
	return true;
}
static_assert( daw_reduce_test_001( ) );

constexpr bool daw_minmax_element_test_001( ) {
	std::array<int, 9> const tst = {1, 3, 2, -1, 100, -50, 1, 5, 2442};
	auto result = daw::algorithm::minmax_element( tst.cbegin( ), tst.cend( ) );

	daw::expecting( -50, *( result.min_element ) );
	daw::expecting( 2442, *( result.max_element ) );
	return true;
}
static_assert( daw_minmax_element_test_001( ) );

constexpr bool daw_satisfies_one_test_001( ) {
	std::array<int, 11> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst,
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 11 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 12 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 13 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 5 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 15 ) != v.cend( );
	  } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( ans );
	return true;
}
static_assert( daw_satisfies_one_test_001( ) );

constexpr bool daw_satisfies_one_test_002( ) {
	std::array<int, 11> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst,
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 11 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 12 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 13 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 5 ) != v.cend( );
	  },
	  []( auto const &v ) {
		  return daw::algorithm::find( v.cbegin( ), v.cend( ), 15 ) != v.cend( );
	  } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( !ans );
	return true;
}
static_assert( daw_satisfies_one_test_002( ) );

constexpr bool daw_satisfies_one_test_003( ) {
	std::array<int, 11> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst.begin( ), tst.end( ), []( auto v ) { return v == 11; },
	  []( auto v ) { return v == 12; }, []( auto v ) { return v == 13; },
	  []( auto v ) { return v == 14; }, []( auto v ) { return v == 15; } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( !ans );
	return true;
}
static_assert( daw_satisfies_one_test_003( ) );

constexpr bool daw_satisfies_one_test_004( ) {
	std::array<int, 11> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_one(
	  tst.begin( ), tst.end( ), []( auto v ) { return v == 11; },
	  []( auto v ) { return v == 12; }, []( auto v ) { return v == 13; },
	  []( auto v ) { return v == 5; }, []( auto v ) { return v == 15; } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( ans );
	return true;
}
static_assert( daw_satisfies_one_test_004( ) );

constexpr bool daw_satisfies_all_test_001( ) {
	std::array<int, 11> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst, []( auto const &v ) { return v.size( ) == 11; },
	  []( auto const &v ) { return v.front( ) == 0; },
	  []( auto const &v ) { return v.back( ) == 10; } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( ans );
	return true;
}
static_assert( daw_satisfies_all_test_001( ) );

constexpr bool daw_satisfies_all_test_002( ) {
	std::array<int, 11> const tst = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst, []( auto const &v ) { return v.size( ) == 11; },
	  []( auto const &v ) { return v.front( ) == 0; },
	  []( auto const &v ) { return v.back( ) == 11; } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( !ans );
	return true;
}
static_assert( daw_satisfies_all_test_002( ) );

constexpr bool daw_satisfies_all_test_003( ) {
	std::array<int, 6> const tst = {0, 2, 4, 6, 8, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst.begin( ), tst.end( ), []( auto v ) { return v < 11; },
	  []( auto v ) { return v >= 0; }, []( auto v ) { return v % 2 == 0; } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( ans );
	return true;
}
static_assert( daw_satisfies_all_test_003( ) );

constexpr bool daw_satisfies_all_test_004( ) {
	std::array<int, 11> const tst = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto const ans = daw::algorithm::satisfies_all(
	  tst.begin( ), tst.end( ), []( auto v ) { return v < 11; },
	  []( auto v ) { return v >= 0; }, []( auto v ) { return v % 2 == 0; } );

	static_assert( daw::is_same_v<bool, daw::remove_cvref_t<decltype( ans )>> );
	daw::expecting( !ans );
	return true;
}
static_assert( daw_satisfies_all_test_004( ) );

constexpr bool daw_in_range_test_001( ) {
	auto tst = daw::algorithm::in_range( 1, 5 );

	daw::expecting( tst( 4 ) );
	daw::expecting( tst( 1 ) );
	daw::expecting( tst( 5 ) );
	daw::expecting( !tst( 0 ) );
	daw::expecting( !tst( 6 ) );
	daw::expecting( !tst( 14 ) );
	return true;
}
static_assert( daw_in_range_test_001( ) );

constexpr bool daw_equal_to_test_001( ) {
	auto tst = daw::algorithm::equal_to( 5 );

	daw::expecting( tst( 5 ) );
	daw::expecting( !tst( -1 ) );
	return true;
}
static_assert( daw_equal_to_test_001( ) );

constexpr bool daw_greater_than_test_001( ) {
	auto tst = daw::algorithm::greater_than( 5 );

	daw::expecting( tst( 6 ) );
	daw::expecting( !tst( 5 ) );
	daw::expecting( !tst( -1 ) );
	return true;
}
static_assert( daw_greater_than_test_001( ) );

constexpr bool daw_greater_than_or_equal_to_test_001( ) {
	auto tst = daw::algorithm::greater_than_or_equal_to( 5 );

	daw::expecting( tst( 6 ) );
	daw::expecting( tst( 5 ) );
	daw::expecting( !tst( -1 ) );
	return true;
}
static_assert( daw_greater_than_or_equal_to_test_001( ) );

constexpr bool daw_less_than_test_001( ) {
	auto tst = daw::algorithm::less_than( 5 );

	daw::expecting( !tst( 6 ) );
	daw::expecting( !tst( 5 ) );
	daw::expecting( tst( -1 ) );
	return true;
}
static_assert( daw_less_than_test_001( ) );

constexpr bool daw_less_than_or_equal_to_test_001( ) {
	auto tst = daw::algorithm::less_than_or_equal_to( 5 );

	daw::expecting( !tst( 6 ) );
	daw::expecting( tst( 5 ) );
	daw::expecting( tst( -1 ) );
	return true;
}
static_assert( daw_less_than_or_equal_to_test_001( ) );

constexpr bool daw_lexigraphical_compare_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 10> const b = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto ans = daw::algorithm::lexicographical_compare( a.cbegin( ), a.cend( ),
	                                                    b.cbegin( ), b.cend( ) );
	daw::expecting( ans );
	return true;
}
static_assert( daw_lexigraphical_compare_test_001( ) );

constexpr bool daw_lexigraphical_compare_test_002( ) {
	std::array<int, 10> const a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 11> const b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto ans = daw::algorithm::lexicographical_compare( a.cbegin( ), a.cend( ),
	                                                    b.cbegin( ), b.cend( ) );
	daw::expecting( !ans );
	return true;
}
static_assert( daw_lexigraphical_compare_test_002( ) );

constexpr bool daw_transform_if_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 6> b{};
	std::array<int, 6> const expected_b = {-1, 1, 3, 5, 7, 9};

	daw::algorithm::transform_if( a.cbegin( ), a.cend( ), b.data( ),
	                              []( auto const &v ) { return v % 2 == 0; },
	                              []( auto const &v ) { return v - 1; } );

	daw::expecting( daw::algorithm::equal(
	  b.cbegin( ), b.cend( ), expected_b.cbegin( ), expected_b.cend( ) ) );
	return true;
}
static_assert( daw_transform_if_test_001( ) );

constexpr bool daw_transform_n_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 4> b{-1, -1, -1, -1};
	std::array<int, 4> const expected_b = {0, 2, 4, 6};

	daw::algorithm::transform_n( a.cbegin( ), b.data( ), 4,
	                             []( auto const &v ) { return v * 2; } );

	daw::expecting( daw::algorithm::equal(
	  b.cbegin( ), b.cend( ), expected_b.cbegin( ), expected_b.cend( ) ) );
	return true;
}
static_assert( daw_transform_n_test_001( ) );

constexpr bool daw_transform_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 11> b{};
	std::array<int, 11> const expected_b = {0,  2,  4,  6,  8, 10,
	                                        12, 14, 16, 18, 20};

	daw::algorithm::transform( a.cbegin( ), a.cend( ), b.data( ),
	                           []( auto const &v ) { return v * 2; } );

	daw::expecting( daw::algorithm::equal(
	  b.cbegin( ), b.cend( ), expected_b.cbegin( ), expected_b.cend( ) ) );
	return true;
}
static_assert( daw_transform_test_001( ) );

constexpr bool daw_transform_it_test_001( ) {
	std::array<int, 6> a = {1, 2, 3, 4, 5, 6};
	std::array<int, 21> b{};
	std::array<int, 21> const expected_b = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5,
	                                        5, 5, 5, 5, 6, 6, 6, 6, 6, 6};
	daw::algorithm::transform_it(
	  a.cbegin( ), a.cend( ), b.data( ), []( int a_val, auto out_it ) {
		  for( size_t n = 0; static_cast<int>( n ) < a_val; ++n ) {
			  *out_it++ = a_val;
		  }
		  return out_it;
	  } );
	daw::expecting( daw::algorithm::equal( expected_b.begin( ), expected_b.end( ),
	                                       b.begin( ) ) );
	return true;
}
static_assert( daw_transform_it_test_001( ) );

constexpr bool daw_copy_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 11> b{};

	daw::algorithm::copy( a.cbegin( ), a.cend( ), b.data( ) );
	daw::expecting(
	  daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
	return true;
}
static_assert( daw_copy_test_001( ) );

constexpr bool daw_copy_n_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 6> b{};
	std::array<int, 6> const expected_b = {0, 1, 2, 3, 4, 5};

	daw::algorithm::copy_n( a.cbegin( ), b.data( ), 6 );
	daw::expecting( daw::algorithm::equal(
	  b.cbegin( ), b.cend( ), expected_b.cbegin( ), expected_b.cend( ) ) );
	return true;
}
static_assert( daw_copy_n_test_001( ) );

constexpr bool daw_move_test_001( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto expected_b = a;
	std::array<int, 11> b{};

	daw::algorithm::move( a.begin( ), a.end( ), b.data( ) );
	daw::expecting( daw::algorithm::equal(
	  expected_b.cbegin( ), expected_b.cend( ), b.cbegin( ), b.cend( ) ) );
	return true;
}
static_assert( daw_move_test_001( ) );

constexpr bool daw_move_n_test_001( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 6> b{};
	std::array<int, 6> const expected_b = {0, 1, 2, 3, 4, 5};

	daw::algorithm::move_n( a.begin( ), b.data( ), 6 );
	daw::expecting( daw::algorithm::equal(
	  b.cbegin( ), b.cend( ), expected_b.cbegin( ), expected_b.cend( ) ) );
	return true;
}
static_assert( daw_move_n_test_001( ) );

constexpr bool daw_equal_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto const b = a;
	daw::expecting(
	  daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ) ) );
	return true;
}
static_assert( daw_equal_test_001( ) );

constexpr bool daw_equal_test_002( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 11> const b = {0, 1, 2, 3, 4, 1, 6, 7, 8, 9, 10};
	daw::expecting(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ) ) );
	return true;
}
static_assert( daw_equal_test_002( ) );

constexpr bool daw_equal_test_003( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto const b = a;
	daw::expecting(
	  daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
	return true;
}
static_assert( daw_equal_test_003( ) );

constexpr bool daw_equal_test_004( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 11> const b = {0, 1, 2, 3, 4, 1, 6, 7, 8, 9, 10};
	daw::expecting(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
	return true;
}
static_assert( daw_equal_test_004( ) );

constexpr bool daw_equal_test_005( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 5> const b = {0, 1, 2, 3, 4};
	daw::expecting(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ) ) );
	return true;
}
static_assert( daw_equal_test_005( ) );

constexpr bool daw_equal_test_006( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 11> const b = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	daw::expecting(
	  daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ),
	                         []( auto lhs, auto rhs ) { return lhs == rhs; } ) );
	return true;
}
static_assert( daw_equal_test_006( ) );

constexpr bool daw_equal_test_007( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 11> const b = {0, 1, 2, 5, 4, 5, 6, 7, 8, 9, 10};
	daw::expecting(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ),
	                          []( auto lhs, auto rhs ) { return lhs == rhs; } ) );
	return true;
}
static_assert( daw_equal_test_007( ) );

constexpr bool daw_equal_test_008( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::array<int, 10> const b = {0, 1, 3, 4, 5, 6, 7, 8, 9, 10};
	daw::expecting(
	  !daw::algorithm::equal( a.cbegin( ), a.cend( ), b.cbegin( ), b.cend( ),
	                          []( auto lhs, auto rhs ) { return lhs == rhs; } ) );
	return true;
}
static_assert( daw_equal_test_008( ) );

constexpr bool daw_rotate_test_001( ) {
	std::array<int, 11> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	daw::algorithm::rotate( a.begin( ), std::next( a.begin( ), 5 ), a.end( ) );
	daw::expecting( 5, a.front( ) );
	daw::expecting( 4, a.back( ) );
	daw::expecting( 10, a[5] );
	return true;
}
static_assert( daw_rotate_test_001( ) );

constexpr bool daw_upper_bound_test_001( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto tst = daw::algorithm::upper_bound( a.cbegin( ), a.cend( ), 5 );
	daw::expecting( tst, std::next( a.cbegin( ), 6 ) );
	return true;
}
static_assert( daw_upper_bound_test_001( ) );

constexpr bool daw_upper_bound_test_002( ) {
	std::array<int, 11> const a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto tst = daw::algorithm::upper_bound( a.cbegin( ), a.cend( ), 20 );
	daw::expecting( tst, a.cend( ) );
	return true;
}
static_assert( daw_upper_bound_test_002( ) );

constexpr bool daw_minmax_item_test_001( ) {
	int a = 5;
	int b = -100;
	auto ans = daw::algorithm::minmax_item( a, b );
	daw::expecting( -100, ans.first );
	daw::expecting( 5, ans.second );
	return true;
}
static_assert( daw_minmax_item_test_001( ) );

constexpr bool daw_minmax_item_test_002( ) {
	struct A {
		int v;
	};
	A a{5};
	A b{-100};
	auto ans = daw::algorithm::minmax_item(
	  a, b, []( auto const &lhs, auto const &rhs ) { return lhs.v < rhs.v; } );
	daw::expecting( -100, ans.first.v );
	daw::expecting( 5, ans.second.v );
	return true;
}
static_assert( daw_minmax_item_test_002( ) );

constexpr bool cartesian_product_test_001( ) {
	std::array<int, 5> a = {1, 2, 3, 4, 5};
	std::array<int, 5> b = {9, 8, 7, 6, 5};
	std::array<int, 5> c = {10, 20, 30, 40, 50};
	// Sum of all is 200
	int sum = 0;
	daw::algorithm::cartesian_product(
	  [&sum]( auto... vals ) { sum += ( vals + ... ); }, begin( a ), end( a ),
	  begin( b ), begin( c ) );

	daw::expecting( 200, sum );
	return true;
}
static_assert( cartesian_product_test_001( ) );

void daw_extract_to_001( ) {
#if defined( __cpp_lib_node_extract )
	std::unordered_map<int, int> a = {{1, 2}, {3, 4}, {5, 6}};
	std::unordered_map<int, int> b{};

	daw::expecting( a.size( ) == 3 );
	daw::expecting( b.empty( ) );

	daw::algorithm::extract_to( a, b, 3 );

	daw::expecting( a.size( ) == 2 );
	daw::expecting( b.size( ) == 1 );

	std::pair<int const, int> c( 3, 4 );

	daw::expecting( !b.empty( ) );
	daw::expecting( *std::begin( b ) == c );

	daw::algorithm::extract_all( a, b );

	daw::expecting( a.empty( ) );
	daw::expecting( b.size( ) == 3 );
#endif
}

int main( ) {
	daw_extract_to_001( );
}
