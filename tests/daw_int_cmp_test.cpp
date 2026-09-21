// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_as.h"
#include "daw/daw_int_cmp.h"

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace {
	template<typename T, typename U, typename = void>
	inline constexpr bool can_compare_v = false;

	template<typename T, typename U>
	inline constexpr bool
	  can_compare_v<T, U,
	                std::void_t<decltype( daw::cmp_equal(
	                  std::declval<T>( ), std::declval<U>( ) ) )>> = true;

	template<typename T, typename U>
	constexpr bool check_equal( T lhs, U rhs ) {
		return daw::cmp_equal( lhs, rhs ) and daw::cmp_equal( rhs, lhs ) and
		       not daw::cmp_not_equal( lhs, rhs ) and
		       not daw::cmp_not_equal( rhs, lhs ) and
		       not daw::cmp_less( lhs, rhs ) and not daw::cmp_less( rhs, lhs ) and
		       not daw::cmp_greater( lhs, rhs ) and
		       not daw::cmp_greater( rhs, lhs ) and
		       daw::cmp_less_equal( lhs, rhs ) and
		       daw::cmp_less_equal( rhs, lhs ) and
		       daw::cmp_greater_equal( lhs, rhs ) and
		       daw::cmp_greater_equal( rhs, lhs );
	}

	template<typename T, typename U>
	constexpr bool check_less( T lhs, U rhs ) {
		return not daw::cmp_equal( lhs, rhs ) and not daw::cmp_equal( rhs, lhs ) and
		       daw::cmp_not_equal( lhs, rhs ) and daw::cmp_not_equal( rhs, lhs ) and
		       daw::cmp_less( lhs, rhs ) and not daw::cmp_less( rhs, lhs ) and
		       not daw::cmp_greater( lhs, rhs ) and daw::cmp_greater( rhs, lhs ) and
		       daw::cmp_less_equal( lhs, rhs ) and
		       not daw::cmp_less_equal( rhs, lhs ) and
		       not daw::cmp_greater_equal( lhs, rhs ) and
		       daw::cmp_greater_equal( rhs, lhs );
	}

	static_assert( can_compare_v<int, unsigned> );
	static_assert( not can_compare_v<double, int> );
	static_assert( not can_compare_v<int, double> );

	static_assert( noexcept( daw::cmp_equal( 0, 0U ) ) );
	static_assert( noexcept( daw::cmp_not_equal( 0, 0U ) ) );
	static_assert( noexcept( daw::cmp_less( 0, 0U ) ) );
	static_assert( noexcept( daw::cmp_greater( 0, 0U ) ) );
	static_assert( noexcept( daw::cmp_less_equal( 0, 0U ) ) );
	static_assert( noexcept( daw::cmp_greater_equal( 0, 0U ) ) );
	static_assert( noexcept( daw::in_range<int>( 0U ) ) );

	// Same-signedness comparisons, including different widths.
	static_assert( check_equal( 0, 0LL ) );
	static_assert( check_equal( std::uint8_t{ 42 }, std::uint64_t{ 42 } ) );
	static_assert( check_less( -2, -1LL ) );
	static_assert( check_less( std::uint8_t{ 1 }, std::uint64_t{ 2 } ) );

	// Mixed signedness is the case these helpers are intended to make safe.
	static_assert( check_equal( 0, 0U ) );
	static_assert( check_equal( 42, 42ULL ) );
	static_assert( check_less( -1, 0U ) );
	static_assert( check_less( -1LL, std::numeric_limits<unsigned>::max( ) ) );
	static_assert( check_less( 7U, 8 ) );
	static_assert( check_less(
	  std::numeric_limits<int>::max( ),
	  daw::as<unsigned long long>( std::numeric_limits<int>::max( ) ) + 1ULL ) );
	static_assert(
	  check_less( std::numeric_limits<long long>::max( ),
	              std::numeric_limits<unsigned long long>::max( ) ) );

	// Range checks include both endpoints and reject values on either side.
	static_assert( daw::in_range<std::int8_t>( std::int8_t{ -128 } ) );
	static_assert( daw::in_range<std::int8_t>( -128 ) );
	static_assert( daw::in_range<std::int8_t>( 127U ) );
	static_assert( not daw::in_range<std::int8_t>( -129 ) );
	static_assert( not daw::in_range<std::int8_t>( 128U ) );
	static_assert( daw::in_range<std::uint8_t>( 0 ) );
	static_assert( daw::in_range<std::uint8_t>( 255 ) );
	static_assert( not daw::in_range<std::uint8_t>( -1 ) );
	static_assert( not daw::in_range<std::uint8_t>( 256 ) );
	static_assert( daw::in_range<std::uint64_t>(
	  std::numeric_limits<std::uint64_t>::max( ) ) );

	static_assert( not daw::signbit( 0 ) );
	static_assert( not daw::signbit( 1 ) );
	static_assert( daw::signbit( -1 ) );
	static_assert( daw::signbit( std::numeric_limits<long long>::lowest( ) ) );
	static_assert( not daw::signbit( std::numeric_limits<unsigned>::max( ) ) );

#if defined( DAW_HAS_INT128 )
	using int128 = daw::int128_t;
	using uint128 = daw::uint128_t;

	inline constexpr uint128 uint128_one = daw::as<uint128>( 1 );
	inline constexpr uint128 uint128_high = uint128_one << 100U;
	inline constexpr int128 int128_high = daw::as<int128>( uint128_high );

	static_assert( check_equal( int128_high, uint128_high ) );
	static_assert( check_less( daw::as<int128>( -1 ), uint128{ 0 } ) );
	static_assert( check_less( std::numeric_limits<std::uint64_t>::max( ),
	                           uint128_high ) );
	static_assert( check_less( std::numeric_limits<std::int64_t>::max( ),
	                           int128_high ) );
	static_assert( check_less( daw::max_value<int128>,
	                           daw::max_value<uint128> ) );

	static_assert( daw::in_range<int128>( daw::lowest_value<int128> ) );
	static_assert( daw::in_range<int128>( daw::max_value<int128> ) );
	static_assert( daw::in_range<uint128>( daw::max_value<uint128> ) );
	static_assert( not daw::in_range<int128>( daw::max_value<uint128> ) );
	static_assert( not daw::in_range<uint128>( daw::as<int128>( -1 ) ) );
	static_assert( not daw::in_range<std::uint64_t>( uint128_high ) );

	static_assert( daw::signbit( daw::as<int128>( -1 ) ) );
	static_assert( not daw::signbit( int128{ 0 } ) );
	static_assert( not daw::signbit( daw::max_value<uint128> ) );
#endif
} // namespace

int main( ) {}
