// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

// iota.h first so that this test also checks that it stands on its own
#include "daw/pipelines/iota.h"

#include "daw/daw_ensure.h"
#include "daw/daw_pipelines.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

using namespace daw::pipelines;

namespace {
	// The difference of two iterators is exact, whichever is larger and whether
	// or not the difference fits in T
	template<typename T>
	void check_difference( ) {
		constexpr auto lo = std::numeric_limits<T>::min( );
		constexpr auto hi = std::numeric_limits<T>::max( );
		auto diff = []( T a, T b ) {
			return iota_iterator<T>( a ) - iota_iterator<T>( b );
		};
		daw_ensure( diff( T{ 0 }, T{ 0 } ) == 0 );
		daw_ensure( diff( T{ 10 }, T{ 0 } ) == 10 );
		daw_ensure( diff( T{ 0 }, T{ 10 } ) == -10 );
		daw_ensure( diff( hi, static_cast<T>( hi - 1 ) ) == 1 );
		daw_ensure( diff( static_cast<T>( hi - 1 ), hi ) == -1 );
		daw_ensure( diff( static_cast<T>( lo + 1 ), lo ) == 1 );
		daw_ensure( diff( lo, static_cast<T>( lo + 1 ) ) == -1 );
		if constexpr( sizeof( T ) < sizeof( std::ptrdiff_t ) ) {
			// the whole range fits in std::ptrdiff_t
			daw_ensure( diff( hi, lo ) == static_cast<std::ptrdiff_t>( hi ) -
			                                static_cast<std::ptrdiff_t>( lo ) );
			daw_ensure( diff( lo, hi ) == static_cast<std::ptrdiff_t>( lo ) -
			                                static_cast<std::ptrdiff_t>( hi ) );
		}
	}

	// Moving an iterator past either end of T wraps, it is not undefined
	template<typename T>
	void check_wraparound( ) {
		constexpr auto lo = std::numeric_limits<T>::min( );
		constexpr auto hi = std::numeric_limits<T>::max( );
		auto at = []( T v ) {
			return iota_iterator<T>( v );
		};
		daw_ensure( *( at( hi ) + 1 ) == lo );
		daw_ensure( *( 1 + at( hi ) ) == lo );
		daw_ensure( *( at( lo ) - 1 ) == hi );
		daw_ensure( at( hi )[1] == lo );
		daw_ensure( at( T{ 5 } )[3] == T{ 8 } );
		{
			auto it = at( hi );
			it += 1;
			daw_ensure( *it == lo );
			it -= 1;
			daw_ensure( *it == hi );
		}
		if constexpr( sizeof( T ) < sizeof( std::ptrdiff_t ) ) {
			// a whole number of laps around T is no movement at all
			constexpr auto lap = std::ptrdiff_t{ 1 } << ( sizeof( T ) * CHAR_BIT );
			daw_ensure( *( at( T{ 3 } ) + lap ) == T{ 3 } );
			daw_ensure( *( at( T{ 3 } ) - lap ) == T{ 3 } );
		}
	}

	template<typename T>
	void check_type( ) {
		check_difference<T>( );
		check_wraparound<T>( );
	}

	void test_iota_view_full_range_int( ) {
		auto v = iota_view<int>{ INT_MIN, INT_MAX };
		if constexpr( sizeof( std::ptrdiff_t ) >= 8 ) {
			daw_ensure( v.end( ) - v.begin( ) == 4294967295LL );
		}
		auto const first = pipeline( v, Take( 3 ), To<std::vector> );
		daw_ensure( first ==
		            std::vector<int>{ INT_MIN, INT_MIN + 1, INT_MIN + 2 } );
		auto const strided = pipeline( v, Every( 1'000'000'000 ), To<std::vector> );
		daw_ensure( strided.size( ) == 5 );
		daw_ensure( strided.front( ) == INT_MIN );
		daw_ensure( strided.back( ) == 1'852'516'352 );
	}

	void test_iota_view_unsigned_reverse_difference( ) {
		auto v = iota_view<std::uint32_t>{ 0, 10 };
		daw_ensure( v.end( ) - v.begin( ) == 10 );
		daw_ensure( v.begin( ) - v.end( ) == -10 );
	}

	// The length does not fit in std::ptrdiff_t, so it is unbounded
	void test_iota_view_unbounded_take( ) {
		auto v =
		  iota_view<std::size_t>{ 0, std::numeric_limits<std::size_t>::max( ) };
		auto const first = pipeline( v, Take( 3 ), To<std::vector> );
		daw_ensure( first == std::vector<std::size_t>{ 0, 1, 2 } );
	}

	// The wide unsigned type that the reference sums are done in
#if defined( DAW_HAS_INT128 )
	using wide_uint_t = daw::uint128_t;
	using wide_int_t = daw::int128_t;
#else
	using wide_uint_t = std::uint64_t;
	using wide_int_t = std::int64_t;
#endif

	// v + n wrapped around T, found without going through the iterator
	template<typename T, typename N>
	constexpr T reference_sum( T v, N n ) {
		return static_cast<T>(
		  static_cast<wide_uint_t>( static_cast<wide_int_t>( v ) ) +
		  static_cast<wide_uint_t>( static_cast<wide_int_t>( n ) ) );
	}

	template<typename T, typename N>
	constexpr T reference_difference( T v, N n ) {
		return static_cast<T>(
		  static_cast<wide_uint_t>( static_cast<wide_int_t>( v ) ) -
		  static_cast<wide_uint_t>( static_cast<wide_int_t>( n ) ) );
	}

	// Constant evaluation rejects signed overflow, so these fail to compile if
	// the arithmetic can overflow, without needing a sanitizer
	static_assert( *( iota_iterator<std::int32_t>( 1 ) +
	                  std::numeric_limits<std::int64_t>::max( ) ) ==
	               reference_sum( std::int32_t{ 1 },
	                              std::numeric_limits<std::int64_t>::max( ) ) );
	static_assert(
	  *( iota_iterator<std::int32_t>( 0 ) -
	     std::numeric_limits<std::int64_t>::min( ) ) ==
	  reference_difference( std::int32_t{ 0 },
	                        std::numeric_limits<std::int64_t>::min( ) ) );
	static_assert( *( iota_iterator<std::int16_t>( 1 ) +
	                  std::numeric_limits<std::int32_t>::max( ) ) ==
	               reference_sum( std::int16_t{ 1 },
	                              std::numeric_limits<std::int32_t>::max( ) ) );
	static_assert( *( iota_iterator<std::int8_t>( -128 ) +
	                  std::numeric_limits<std::int16_t>::min( ) ) ==
	               reference_sum( std::int8_t{ -128 },
	                              std::numeric_limits<std::int16_t>::min( ) ) );
	static_assert( iota_iterator<std::int32_t>(
	                 1 )[std::numeric_limits<std::uint64_t>::max( )] ==
	               reference_sum( std::int32_t{ 1 },
	                              std::numeric_limits<std::uint64_t>::max( ) ) );

	// Steps as large as the iterator's own difference_type allows.  The result
	// wraps around T, the arithmetic that finds it must not overflow
	template<typename T>
	void check_extreme_steps( ) {
		using it_t = iota_iterator<T>;
		using diff_t = typename it_t::difference_type;
		using size_t_ = typename it_t::size_type;
		constexpr auto dmax = daw::max_value<diff_t>;
		constexpr auto dmin = daw::min_value<diff_t>;
		constexpr auto lo = std::numeric_limits<T>::min( );
		constexpr auto hi = std::numeric_limits<T>::max( );
		T const values[] = { lo,
		                     static_cast<T>( lo + 1 ),
		                     T{ 0 },
		                     T{ 1 },
		                     static_cast<T>( hi - 1 ),
		                     hi };
		diff_t const steps[] = { dmin,
		                         static_cast<diff_t>( dmin + 1 ),
		                         diff_t{ -1 },
		                         diff_t{ 0 },
		                         diff_t{ 1 },
		                         static_cast<diff_t>( dmax - 1 ),
		                         dmax };
		for( T v : values ) {
			for( diff_t n : steps ) {
				{
					auto it = it_t( v );
					it += n;
					daw_ensure( *it == reference_sum( v, n ) );
				}
				{
					auto it = it_t( v );
					it -= n;
					daw_ensure( *it == reference_difference( v, n ) );
				}
				daw_ensure( *( it_t( v ) + n ) == reference_sum( v, n ) );
				daw_ensure( *( n + it_t( v ) ) == reference_sum( v, n ) );
				daw_ensure( *( it_t( v ) - n ) == reference_difference( v, n ) );
				// the index is unsigned, a negative step is a huge index
				auto const k = static_cast<size_t_>( n );
				daw_ensure( it_t( v )[k] == reference_sum( v, k ) );
			}
		}
	}

#if defined( DAW_HAS_INT128 )
	// The arithmetic must keep every bit of a value wider than 64 bits
	void test_iota_iterator_int128( ) {
		using i128 = daw::int128_t;
		auto const big = static_cast<i128>( 1 ) << 100;
		auto at = []( i128 v ) {
			return iota_iterator<i128>( v );
		};
		daw_ensure( *( at( big ) + 1 ) == big + 1 );
		daw_ensure( *( 10 + at( big ) ) == big + 10 );
		daw_ensure( *( at( big ) - 3 ) == big - 3 );
		daw_ensure( at( big )[5] == big + 5 );
		{
			auto it = at( big );
			it += 1;
			daw_ensure( *it == big + 1 );
			it -= 2;
			daw_ensure( *it == big - 1 );
		}
		daw_ensure( at( big ) - at( big - 7 ) == 7 );
		daw_ensure( at( big - 7 ) - at( big ) == -7 );
	}
#endif
} // namespace

int main( ) {
	check_type<std::int8_t>( );
	check_type<std::uint8_t>( );
	check_type<std::int16_t>( );
	check_type<std::uint16_t>( );
	check_type<std::int32_t>( );
	check_type<std::uint32_t>( );
	check_type<std::int64_t>( );
	check_type<std::uint64_t>( );
	check_type<char>( );
	check_type<wchar_t>( );
	check_type<char16_t>( );
	check_type<char32_t>( );
	test_iota_view_full_range_int( );
	test_iota_view_unsigned_reverse_difference( );
	test_iota_view_unbounded_take( );
	check_extreme_steps<std::int8_t>( );
	check_extreme_steps<std::uint8_t>( );
	check_extreme_steps<std::int16_t>( );
	check_extreme_steps<std::uint16_t>( );
	check_extreme_steps<std::int32_t>( );
	check_extreme_steps<std::uint32_t>( );
	check_extreme_steps<char>( );
	check_extreme_steps<wchar_t>( );
	check_extreme_steps<char16_t>( );
	check_extreme_steps<char32_t>( );
#if defined( DAW_HAS_INT128 )
	check_extreme_steps<std::int64_t>( );
	check_extreme_steps<std::uint64_t>( );
	test_iota_iterator_int128( );
#endif
}
