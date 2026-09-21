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
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <limits>
#include <ranges>
#include <type_traits>
#include <vector>

// countdown_view{ from, to } is from, from - 1, ... to + 1.  The first is
// included and the last is not, as in iota_view, so to is never produced and
// neither is the lowest value of T.  countdown_view{ 5, 0 } is 5, 4, 3, 2, 1
// and countdown_view{ n } is n down to 1.  It is the reverse of
// iota_view{ to + 1, from + 1 }
//
// daw_ensure ends the program without a message, so main prints the name of
// each test before it runs.  The last name printed is the one that failed.
// Pass part of a name as the argument to run only those tests.
//
// The tests named issueN_ are for the problems that were found in countdown, N
// is the number in the list of them.  daw_dbg_ensure in the view's constructors
// and operator[] is only active without NDEBUG, so issues 2 and 4 need a debug
// build to be seen

using namespace daw::pipelines;

namespace {
	static_assert( std::random_access_iterator<countdown_iterator<int>> );
	static_assert( std::ranges::random_access_range<countdown_view<int>> );

	// The wide types that the reference arithmetic is done in
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

	// The elements of countdown_view<T>{ high, low }, high down to low + 1
	template<typename T>
	std::vector<T> reference_countdown( T high, T low ) {
		auto result = std::vector<T>( );
		for( wide_int_t i = static_cast<wide_int_t>( high );
		     i > static_cast<wide_int_t>( low );
		     --i ) {
			result.push_back( static_cast<T>( i ) );
		}
		return result;
	}

	// The elements of a view by walking it, with == and ++ only, so it does not
	// depend on the difference of the iterators
	template<typename V>
	std::vector<typename V::value_type> walk( V const &v ) {
		auto result = std::vector<typename V::value_type>( );
		for( auto it = v.begin( ); it != v.end( ); ++it ) {
			// an end that is never reached
			daw_ensure( result.size( ) < 100'000 );
			result.push_back( *it );
		}
		return result;
	}

	// The compiler must not be able to fold a check that is wrong into an error
	// at compile time, so that the rest of the tests can still be run. daw_ensure
	// on a constant does not fail when the program runs
	inline volatile int opaque_zero = 0;

	inline bool opaque( bool b ) {
		return static_cast<bool>( static_cast<int>( b ) + opaque_zero );
	}

	// daw_ensure of something the compiler can evaluate is a compile time result,
	// an error for one that is wrong and, with clang, for one that is right.
	// Every check here fails when the test runs instead
#define countdown_ensure( ... ) \
	daw_ensure( opaque( static_cast<bool>( __VA_ARGS__ ) ) )

	template<typename T>
	using type_tag = std::type_identity<T>;

	template<typename F>
	void for_each_int( F &&f ) {
		f( type_tag<std::int8_t>{ } );
		f( type_tag<std::uint8_t>{ } );
		f( type_tag<std::int16_t>{ } );
		f( type_tag<std::uint16_t>{ } );
		f( type_tag<std::int32_t>{ } );
		f( type_tag<std::uint32_t>{ } );
		f( type_tag<std::int64_t>{ } );
		f( type_tag<std::uint64_t>{ } );
	}

	template<typename F>
	void for_each_int_and_char( F &&f ) {
		for_each_int( f );
		f( type_tag<char>{ } );
		f( type_tag<wchar_t>{ } );
		f( type_tag<char16_t>{ } );
		f( type_tag<char32_t>{ } );
	}

	// ---- Issue 2: the constructors asserted the wrong way round.  from is the
	// higher end, from >= to ----
	void issue2_ctor_accepts_high_then_low( ) {
		for_each_int( []( auto id ) {
			using T = typename decltype( id )::type;
			constexpr auto lo = std::numeric_limits<T>::min( );
			constexpr auto hi = std::numeric_limits<T>::max( );
			countdown_ensure( not countdown_view<T>( T{ 5 }, T{ 0 } ).empty( ) );
			countdown_ensure( countdown_view<T>( T{ 5 }, T{ 5 } ).empty( ) );
			countdown_ensure( countdown_view<T>( T{ 0 }, T{ 0 } ).empty( ) );
			countdown_ensure( not countdown_view<T>( hi, lo ).empty( ) );
			countdown_ensure( countdown_view<T>( lo, lo ).empty( ) );
			countdown_ensure( countdown_view<T>( hi, hi ).empty( ) );
		} );
	}

	// ---- Issue 3: the single argument constructor is from, not to.
	// countdown_view{ 5 } is 5, 4, 3, 2, 1 ----
	void issue3_single_argument_is_the_high_end( ) {
		for_each_int( []( auto id ) {
			using T = typename decltype( id )::type;
			auto const v = countdown_view<T>( T{ 5 } );
			countdown_ensure( not v.empty( ) );
			countdown_ensure( *v.begin( ) == T{ 5 } );
			countdown_ensure( walk( v ) == std::vector<T>{ 5, 4, 3, 2, 1 } );
			countdown_ensure( countdown_view<T>( T{ 1 } ).begin( ) !=
			                  countdown_view<T>( T{ 1 } ).end( ) );
			countdown_ensure( walk( countdown_view<T>( T{ 1 } ) ) ==
			                  std::vector<T>{ 1 } );
			countdown_ensure( countdown_view<T>( T{ 0 } ).empty( ) );
			countdown_ensure( walk( countdown_view<T>( T{ 0 } ) ).empty( ) );
			countdown_ensure( countdown_view<T>( T{ 5 } ) ==
			                  countdown_view<T>( T{ 5 }, T{ 0 } ) );
		} );
	}

	// ---- Issue 1: the difference of two iterators had the wrong sign. Advancing
	// is the positive direction, so end - begin is the length ----

#if defined( DAW_HAS_INT128 )
	// The difference must keep every bit of a value wider than 64 bits
	void check_difference_int128( ) {
		using i128 = daw::int128_t;
		auto const big = ( static_cast<i128>( 1 ) << 100 ) + opaque_zero;
		auto const it0 = countdown_iterator<i128>( big );
		countdown_ensure( ( it0 + 7 ) - it0 == 7 );
		countdown_ensure( it0 - ( it0 + 7 ) == -7 );
	}
#endif

	void issue1_iterator_difference_sign( ) {
#if defined( DAW_HAS_INT128 )
		check_difference_int128( );
#endif
		for_each_int( []( auto id ) {
			using T = typename decltype( id )::type;
			constexpr auto lo = std::numeric_limits<T>::min( );
			constexpr auto hi = std::numeric_limits<T>::max( );
			{
				auto const v = countdown_view<T>( T{ 5 }, T{ 0 } );
				auto const b = v.begin( );
				auto const e = v.end( );
				countdown_ensure( e - b == 5 );
				countdown_ensure( b - e == -5 );
				countdown_ensure( b - b == 0 );
				countdown_ensure( b + 5 == e );
				countdown_ensure( e - 5 == b );
				countdown_ensure( ( b + 3 ) - b == 3 );
				countdown_ensure( b - ( b + 3 ) == -3 );
				countdown_ensure( ( b + 3 ) - ( b + 1 ) == 2 );
				auto it = b;
				++it;
				countdown_ensure( it - b == 1 );
				countdown_ensure( b - it == -1 );
				countdown_ensure( std::distance( b, e ) == 5 );
			}
			{
				// to is the lowest value of T, which is not produced
				auto const v = countdown_view<T>( static_cast<T>( lo + 5 ), lo );
				countdown_ensure( v.end( ) - v.begin( ) == 5 );
				countdown_ensure( v.begin( ) - v.end( ) == -5 );
			}
			{
				auto const v = countdown_view<T>( hi, static_cast<T>( hi - 5 ) );
				countdown_ensure( v.end( ) - v.begin( ) == 5 );
				countdown_ensure( v.begin( ) - v.end( ) == -5 );
			}
			if constexpr( sizeof( wide_int_t ) > sizeof( T ) ) {
				// the whole range of T
				auto const v = countdown_view<T>( hi, lo );
				auto const len =
				  static_cast<wide_int_t>( hi ) - static_cast<wide_int_t>( lo );
				countdown_ensure( static_cast<wide_int_t>( v.end( ) - v.begin( ) ) ==
				                  len );
				countdown_ensure( static_cast<wide_int_t>( v.begin( ) - v.end( ) ) ==
				                  -len );
			}
			{
				// what sizes a range is what fills a container from it
				auto const v = countdown_view<T>( T{ 5 }, T{ 0 } );
				countdown_ensure( pipeline( v, To<std::vector> ) ==
				                  std::vector<T>{ 5, 4, 3, 2, 1 } );
				countdown_ensure( pipeline( v, Take( 2 ), To<std::vector> ) ==
				                  std::vector<T>{ 5, 4 } );
				countdown_ensure( pipeline( v, Every( 2 ), To<std::vector> ) ==
				                  std::vector<T>{ 5, 3, 1 } );
			}
		} );
	}

	// ---- Issue 4: the asserts in the view's operator[] compared the index to
	// the values.  The index is 0 <= n < length ----

	template<typename T>
	void check_subscript( T high, T low ) {
		auto const v = countdown_view<T>( high, low );
		auto const expected = reference_countdown<T>( high, low );
		for( std::size_t n = 0; n < expected.size( ); ++n ) {
			countdown_ensure(
			  v[static_cast<typename countdown_view<T>::difference_type>( n )] ==
			  expected[n] );
		}
	}

	void issue4_view_subscript( ) {
		for_each_int( []( auto id ) {
			using T = typename decltype( id )::type;
			constexpr auto lo = std::numeric_limits<T>::min( );
			constexpr auto hi = std::numeric_limits<T>::max( );
			check_subscript<T>( T{ 5 }, T{ 0 } );
			check_subscript<T>( T{ 10 }, T{ 7 } );
			check_subscript<T>( T{ 10 }, T{ 9 } );
			check_subscript<T>( static_cast<T>( lo + 4 ), lo );
			check_subscript<T>( hi, static_cast<T>( hi - 4 ) );
			auto const v = countdown_view<T>( T{ 10 }, T{ 7 } );
			countdown_ensure( v[0] == T{ 10 } );
			countdown_ensure( v[2] == T{ 8 } );
		} );
	}

	// ---- Issue 5: operator<=> on the view returns bool, where it is used it
	// does not compile.  If the view is ordered the order must be a valid one,
	// and if it is not ordered there is nothing to check ----
	template<typename V, typename T>
	void check_view_ordering( ) {
		// clang-format off
		if constexpr( requires( V const &a, V const &b ) { a <=> b; } ) {
			// clang-format on
			countdown_ensure( std::three_way_comparable<V> );
			if constexpr( std::three_way_comparable<V> ) {
				auto const a = V( T{ 5 }, T{ 0 } );
				auto const b = V( T{ 5 }, T{ 2 } );
				auto const c = V( T{ 6 }, T{ 0 } );
				countdown_ensure( ( a <=> a ) == 0
				// == and <=> must agree, a and b differ only in their low end
				countdown_ensure( ( a == b ) == ( ( a <=> b ) == 0 ) );
				countdown_ensure( ( a == c ) == ( ( a <=> c ) == 0 ) );
				countdown_ensure( not( a < a ) );
				countdown_ensure( ( a < b ) != ( b < a ) );
				countdown_ensure( ( a < c ) != ( c < a ) );
			}
		}
	}

	void issue5_view_ordering_is_valid( ) {
		for_each_int( []( auto id ) {
			using T = typename decltype( id )::type;
			check_view_ordering<countdown_view<T>, T>( );
		} );
	}

	// ---- Issue 6: operator* was m_value - 1, and that overflowed a signed int
	// at the lowest value of T.  The iterator now holds the element, so there is
	// nothing in operator* to overflow.  A constant expression can not overflow,
	// so this is not constant if it can, without needing a sanitizer ----
	template<typename T>
	concept deref_at_lowest_is_constant = requires {
		typename std::bool_constant<(
		  *countdown_iterator<T>( std::numeric_limits<T>::min( ) ) ==
		  std::numeric_limits<T>::min( ) )>;
	};

	void iterator_deref_and_wrap_at_the_ends( ) {
		for_each_int_and_char( []( auto id ) {
			using T = typename decltype( id )::type;
			constexpr auto lo = std::numeric_limits<T>::min( );
			constexpr auto hi = std::numeric_limits<T>::max( );
			countdown_ensure( deref_at_lowest_is_constant<T> );
			countdown_ensure( *countdown_iterator<T>( lo ) == lo );
			countdown_ensure( *countdown_iterator<T>( hi ) == hi );
			// stepping past the lowest wraps to the highest
			countdown_ensure( *( countdown_iterator<T>( lo ) + 1 ) == hi );
			countdown_ensure( *( countdown_iterator<T>( hi ) - 1 ) == lo );
			auto const it = countdown_iterator<T>( static_cast<T>( lo + 1 ) );
			countdown_ensure( *( it + 1 ) == lo );
			countdown_ensure( *( it + 2 ) == hi );
		} );
	}

	// ---- What works now, so it stays working while the above is fixed ----

	// These are relative to where the iterator starts, so they do not depend on
	// its value
	void iterator_step_and_order( ) {
		for_each_int_and_char( []( auto id ) {
			using T = typename decltype( id )::type;
			using it_t = countdown_iterator<T>;
			auto const it0 = it_t( T{ 10 } );
			auto const base = *it0;
			{
				auto it = it0;
				countdown_ensure( *++it == static_cast<T>( base - 1 ) );
				countdown_ensure( *it == static_cast<T>( base - 1 ) );
				countdown_ensure( *--it == base );
			}
			{
				auto it = it0;
				auto const before = it++;
				countdown_ensure( before == it0 );
				countdown_ensure( *before == base );
				countdown_ensure( *it == static_cast<T>( base - 1 ) );
				auto const after = it--;
				countdown_ensure( *after == static_cast<T>( base - 1 ) );
				countdown_ensure( it == it0 );
			}
			// advancing is later, the value falls
			auto const it1 = it0 + 1;
			countdown_ensure( it0 < it1 );
			countdown_ensure( it0 <= it1 );
			countdown_ensure( it1 > it0 );
			countdown_ensure( it1 >= it0 );
			countdown_ensure( it0 != it1 );
			countdown_ensure( not( it1 < it0 ) );
			countdown_ensure( not( it0 < it0 ) );
			countdown_ensure( it0 <= it0 );
			countdown_ensure( it0 == it0 );
		} );
	}

	void iterator_random_access_arithmetic( ) {
		for_each_int_and_char( []( auto id ) {
			using T = typename decltype( id )::type;
			using it_t = countdown_iterator<T>;
			auto const it0 = it_t( T{ 20 } );
			auto const base = *it0;
			countdown_ensure( *( it0 + 3 ) == static_cast<T>( base - 3 ) );
			countdown_ensure( *( 3 + it0 ) == static_cast<T>( base - 3 ) );
			countdown_ensure( *( it0 - 3 ) == static_cast<T>( base + 3 ) );
			countdown_ensure( it0[3] == static_cast<T>( base - 3 ) );
			countdown_ensure( it0[0] == base );
			{
				auto it = it0;
				it += 3;
				countdown_ensure( *it == static_cast<T>( base - 3 ) );
				it -= 3;
				countdown_ensure( it == it0 );
				it -= 2;
				countdown_ensure( *it == static_cast<T>( base + 2 ) );
			}
			countdown_ensure( ( it0 + 3 ) - 3 == it0 );
			countdown_ensure( ( it0 - 3 ) + 3 == it0 );
			if constexpr( sizeof( T ) < sizeof( std::ptrdiff_t ) ) {
				// a whole number of laps around T is no movement at all
				constexpr auto lap = std::ptrdiff_t{ 1 } << ( sizeof( T ) * CHAR_BIT );
				countdown_ensure( it0 + lap == it0 || *( it0 + lap ) == base );
				countdown_ensure( *( it0 + lap ) == base );
				countdown_ensure( *( it0 - lap ) == base );
				countdown_ensure( it0[lap] == base );
			}
		} );
	}

	// Steps as large as the iterator's own difference_type allows.  The result
	// wraps around T, the arithmetic that finds it must not overflow
	template<typename T>
	void check_extreme_steps( ) {
		using it_t = countdown_iterator<T>;
		using diff_t = typename it_t::difference_type;
		constexpr auto dmax = daw::max_value<diff_t>;
		constexpr auto dmin = daw::min_value<diff_t>;
		constexpr auto lo = std::numeric_limits<T>::min( );
		constexpr auto hi = std::numeric_limits<T>::max( );
		T const values[] = {
		  static_cast<T>( lo + 1 ), T{ 0 }, T{ 1 }, static_cast<T>( hi - 1 ), hi };
		diff_t const steps[] = { dmin,
		                         static_cast<diff_t>( dmin + 1 ),
		                         diff_t{ -1 },
		                         diff_t{ 0 },
		                         diff_t{ 1 },
		                         static_cast<diff_t>( dmax - 1 ),
		                         dmax };
		for( T v : values ) {
			auto const it0 = it_t( v );
			auto const base = *it0;
			for( diff_t n : steps ) {
				// advancing lowers the value
				auto const down = reference_difference( base, n );
				auto const up = reference_sum( base, n );
				{
					auto it = it0;
					it += n;
					countdown_ensure( *it == down );
				}
				{
					auto it = it0;
					it -= n;
					countdown_ensure( *it == up );
				}
				countdown_ensure( *( it0 + n ) == down );
				countdown_ensure( *( n + it0 ) == down );
				countdown_ensure( *( it0 - n ) == up );
				countdown_ensure( it0[n] == down );
			}
		}
	}

	void iterator_extreme_steps( ) {
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
#endif
	}

	// A constant expression can not overflow, so these are not constant if the
	// arithmetic can overflow, without needing a sanitizer
	template<typename T, auto N>
	concept steps_are_constant = requires {
		typename std::bool_constant<(
		  *( countdown_iterator<T>( T{ 1 } ) + N ) ==
		    reference_difference( *countdown_iterator<T>( T{ 1 } ), N ) and
		  *( countdown_iterator<T>( T{ 1 } ) - N ) ==
		    reference_sum( *countdown_iterator<T>( T{ 1 } ), N ) and
		  countdown_iterator<T>( T{ 1 } )[N] ==
		    reference_difference( *countdown_iterator<T>( T{ 1 } ), N ) )>;
	};

	void iterator_steps_are_constant_expressions( ) {
		using i16 = std::numeric_limits<std::int16_t>;
		using i32 = std::numeric_limits<std::int32_t>;
		using i64 = std::numeric_limits<std::int64_t>;
		countdown_ensure( steps_are_constant<std::int32_t, i64::max( )> );
		countdown_ensure( steps_are_constant<std::int32_t, i64::min( )> );
		countdown_ensure( steps_are_constant<std::int16_t, i32::max( )> );
		countdown_ensure( steps_are_constant<std::int8_t, i16::min( )> );
		countdown_ensure( steps_are_constant<std::uint32_t, i64::min( )> );
	}

#if defined( DAW_HAS_INT128 )
	// The arithmetic must keep every bit of a value wider than 64 bits
	void iterator_int128( ) {
		using i128 = daw::int128_t;
		auto const big = ( static_cast<i128>( 1 ) << 100 ) + opaque_zero;
		auto const it0 = countdown_iterator<i128>( big );
		auto const base = *it0;
		countdown_ensure( *( it0 + 1 ) == base - 1 );
		countdown_ensure( *( 10 + it0 ) == base - 10 );
		countdown_ensure( *( it0 - 3 ) == base + 3 );
		countdown_ensure( it0[5] == base - 5 );
		{
			auto it = it0;
			it += 1;
			countdown_ensure( *it == base - 1 );
			it -= 2;
			countdown_ensure( *it == base + 1 );
		}
	}
#endif

	// ---- The view against the reverse of the iota ----
	template<typename T>
	void check_view_is_reverse( T high, T low ) {
		auto const v = countdown_view<T>( high, low );
		auto const expected = reference_countdown<T>( high, low );
		countdown_ensure( walk( v ) == expected );
		countdown_ensure( v.empty( ) == expected.empty( ) );
		countdown_ensure( static_cast<std::size_t>( v.end( ) - v.begin( ) ) ==
		                  expected.size( ) );
		countdown_ensure( pipeline( v, To<std::vector> ) == expected );
	}

	void view_is_reverse_of_iota( ) {
		for_each_int( []( auto id ) {
			using T = typename decltype( id )::type;
			constexpr auto lo = std::numeric_limits<T>::min( );
			constexpr auto hi = std::numeric_limits<T>::max( );
			check_view_is_reverse<T>( T{ 10 }, T{ 0 } );
			check_view_is_reverse<T>( T{ 10 }, T{ 7 } );
			check_view_is_reverse<T>( T{ 4 }, T{ 3 } );
			check_view_is_reverse<T>( T{ 3 }, T{ 3 } );
			check_view_is_reverse<T>( static_cast<T>( lo + 5 ), lo );
			check_view_is_reverse<T>( hi, static_cast<T>( hi - 5 ) );
			if constexpr( sizeof( T ) == 1 ) {
				check_view_is_reverse<T>( hi, lo );
			}
		} );
		// the pipeline it is the reverse of
		auto const expected =
		  pipeline( iota_view<int>{ 4, 11 }, ReverseView, To<std::vector> );
		static_assert( std::is_same_v<DAW_TYPEOF( expected ), std::vector<int>> );
		countdown_ensure(
		  pipeline( countdown_view<int>{ 10, 3 }, To<std::vector> ) == expected );
		countdown_ensure( walk( countdown_view<int>{ 10, 3 } ) == expected );
	}

	void view_full_range_int( ) {
		auto const v = countdown_view<int>{ INT_MAX, INT_MIN };
		countdown_ensure( *v.begin( ) == INT_MAX );
		if constexpr( sizeof( std::ptrdiff_t ) >= 8 ) {
			countdown_ensure( v.end( ) - v.begin( ) == 4294967295LL );
		}
		auto const first = pipeline( v, Take( 3 ), To<std::vector> );
		countdown_ensure( first ==
		                  std::vector<int>{ INT_MAX, INT_MAX - 1, INT_MAX - 2 } );
		auto const strided = pipeline( v, Every( 1'000'000'000 ), To<std::vector> );
		countdown_ensure( strided.size( ) == 5 );
		countdown_ensure( strided.front( ) == INT_MAX );
		countdown_ensure( strided.back( ) == -1'852'516'353 );
	}

	// The length does not fit in std::ptrdiff_t, so it is unbounded
	void view_unbounded_take( ) {
		constexpr auto hi = std::numeric_limits<std::size_t>::max( );
		auto const v = countdown_view<std::size_t>{ hi, 0 };
		auto const first = pipeline( v, Take( 3 ), To<std::vector> );
		countdown_ensure( first == std::vector<std::size_t>{ hi, hi - 1, hi - 2 } );
	}

	void view_equality( ) {
		for_each_int( []( auto id ) {
			using T = typename decltype( id )::type;
			countdown_ensure( countdown_view<T>( T{ 5 }, T{ 2 } ) ==
			                  countdown_view<T>( T{ 5 }, T{ 2 } ) );
			countdown_ensure( not( countdown_view<T>( T{ 5 }, T{ 2 } ) ==
			                       countdown_view<T>( T{ 6 }, T{ 2 } ) ) );
			countdown_ensure( not( countdown_view<T>( T{ 5 }, T{ 2 } ) ==
			                       countdown_view<T>( T{ 5 }, T{ 3 } ) ) );
		} );
	}

	struct test_case_t {
		char const *name;
		void ( *fn )( );
	};

	// The issue tests first.  2 is first as a debug build asserts in the
	// constructor for everything after it
	constexpr test_case_t test_cases[] = {
	  { "issue2_ctor_accepts_high_then_low", issue2_ctor_accepts_high_then_low },
	  { "issue3_single_argument_is_the_high_end",
	    issue3_single_argument_is_the_high_end },
	  { "issue1_iterator_difference_sign", issue1_iterator_difference_sign },
	  { "issue4_view_subscript", issue4_view_subscript },
	  { "issue5_view_ordering_is_valid", issue5_view_ordering_is_valid },
	  { "iterator_deref_and_wrap_at_the_ends",
	    iterator_deref_and_wrap_at_the_ends },
	  { "iterator_step_and_order", iterator_step_and_order },
	  { "iterator_random_access_arithmetic", iterator_random_access_arithmetic },
	  { "iterator_extreme_steps", iterator_extreme_steps },
	  { "iterator_steps_are_constant_expressions",
	    iterator_steps_are_constant_expressions },
#if defined( DAW_HAS_INT128 )
	  { "iterator_int128", iterator_int128 },
#endif
	  { "view_is_reverse_of_iota", view_is_reverse_of_iota },
	  { "view_full_range_int", view_full_range_int },
	  { "view_unbounded_take", view_unbounded_take },
	  { "view_equality", view_equality },
	};
#undef countdown_ensure
} // namespace

int main( int argc, char **argv ) {
	for( auto const &tc : test_cases ) {
		if( argc > 1 and std::strstr( tc.name, argv[1] ) == nullptr ) {
			continue;
		}
		std::fprintf( stderr, "[ RUN  ] %s\n", tc.name );
		tc.fn( );
		std::fprintf( stderr, "[  OK  ] %s\n", tc.name );
	}
}
