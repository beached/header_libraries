// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_ensure.h>
#include <daw/daw_pipelines.h>
#include <daw/daw_print.h>

#include <complex>
#include <cstdio>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

using namespace daw::pipelines;

inline constexpr struct AlwaysTrue_t {
	std::true_type constexpr operator( )( auto const &... ) {
		return { };
	}
} AlwaysTrue{ };

static_assert( std::forward_iterator<filter_view<char const *, AlwaysTrue_t>> );

inline constexpr auto vowel =
  IsOneOf<'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U'>;

inline constexpr auto is_letter = []( char c ) {
	return ( 'A' <= c and c <= 'Z' ) or ( 'a' <= c and c <= 'z' );
};

inline constexpr auto to_lower = []( char c ) -> char {
	return c | ' ';
};

int main( ) {
	constexpr auto p = pipeline(
	  Filter( Not( vowel ) ), Take( 8 ), Map( to_lower ), Filter( is_letter )

	);
	constexpr auto p1 = pipeline( p, Take( 4 ) );
	constexpr auto r0 = p1( "Hello World, How are you?" );
	constexpr auto r1 = p1( "Blah blah blah, how is yah?" );
	daw::println( "{}", daw::fmt_range( r0 ) );
	daw::println( "{}", daw::fmt_range( r1 ) );
	constexpr auto q = pipeline( Print,
	                             Filter( Not( vowel ) ),
	                             Take( 8 ),
	                             Map( to_lower ),
	                             Print,
	                             Filter( is_letter ),
	                             Print,
	                             To<std::basic_string>,
	                             Print,
	                             Sort,
	                             Print,
	                             Unique,
	                             Print,
	                             Sum );
	(void)q( "Hello World, How are you?" );
	daw::println( );
	(void)q( "Blah blah blah, how is yah?" );

	// Example from https://youtu.be/itnyR9j8y6E?t=404
	constexpr auto p3 =
	  pipeline( Map( []( auto i ) {
		            return std::pair{ i, std::to_string( i ) };
	            } ),
	            Filter( []( auto const &v ) {
		            auto lhs = static_cast<std::size_t>( v.first );
		            auto const h = std::hash<std::string>{ }( v.second );
		            return lhs >= h;
	            } ),
	            Map( &std::pair<std::size_t, std::string>::second ),
	            Take( 4 ) );

	for( auto s : p3( iota_view( 1, 10001 ) ) ) {
		daw::println( "{}", s );
	}

	auto r2 = pipeline( iota_view( 1, 10001 ),
	                    Map( []( auto i ) {
		                    return std::pair{ i, std::to_string( i ) };
	                    } ),
	                    Filter( []( auto const &v ) {
		                    auto lhs = static_cast<std::size_t>( v.first );
		                    auto const h = std::hash<std::string>{ }( v.second );
		                    return lhs >= h;
	                    } ),
	                    Map( &std::pair<std::size_t, std::string>::second ),
	                    Take( 4 ) );
	for( auto s : r2 ) {
		daw::println( "{}", s );
	}

	auto const p4 = pipeline( Filter( []( int x ) {
		                          return x % 2 == 0;
	                          } ),
	                          Map( []( std::optional<int> x ) {
		                          if( not x ) {
			                          return iota_view<int>{ 0 };
		                          }
		                          return iota_view<int>{ *x };
	                          } ),
	                          Count,
	                          ToIota,
	                          Map( []( int x ) {
		                          return x * 2;
	                          } ) );
	auto x = p4( std::array{ 1, 2, 3 } );
	auto y = *std::next( x.begin( ) );
	(void)y;

	static constexpr auto to_letter = []( auto i ) -> char {
		if( i < 26 ) {
			return static_cast<char>( 'a' + i );
		} else if( i < 52 ) {
			return static_cast<char>( 'A' + ( i - 26 ) );
		}
		throw std::out_of_range( "i is out of range" );
	};

	constexpr auto tp0 =
	  pipeline( Map( []( unsigned i ) {
		            return std::tuple( i, to_letter( i ) );
	            } ),
	            ForEachApply( []( signed i, char c ) {
		            daw::println( "{}, {}", i, c );
	            } ),
	            Swizzle<1, 0, 0>,
	            ForEachApply( []( char c, signed i, signed j ) {
		            daw::println( "{}, {}, {}", c, i, j );
	            } ) );
	(void)tp0( iota_view<unsigned>( 0, 52 ) );

	constexpr auto tp1 = pipeline( Zip( iota_view<char>( 'A', 'Z' ) ),
	                               ForEachApply( []( char a, char b ) {
		                               daw::println( "{}, {}", a, b );
	                               } ) );
	(void)tp1( iota_view<char>( 'a', 'z' ) );

	static constexpr auto prices = std::array{ 100, 200, 150, 180, 130 };
	static constexpr auto costs = std::array{ 10, 20, 50, 40, 100 };
	static constexpr auto const v = pipeline( zip_view( prices, costs ),
	                                          MapApply( []( auto pr, auto co ) {
		                                          return pr - co;
	                                          } ),
	                                          Clamp( 5, 100 ),
	                                          To<std::array<int, 5>> );
	daw::println( "MapApply, Clamp, To<std::vector>\n{}", daw::fmt_range( v ) );

	constexpr auto v1 = pipeline( zip_view( prices, costs, v ), Concat );
	daw::println( "Concat\n{}", daw::fmt_range( v1 ) );

	auto to_array = []<std::size_t ArraySize, typename C>(
	                  std::integral_constant<std::size_t, ArraySize>,
	                  C const &c ) {
		using value_t = typename C::value_type;
		return To<std::array<value_t, ArraySize>>( )( c );
	};

	auto va = to_array( std::integral_constant<std::size_t, 15>{ }, v );

	auto da = std::array{ 1024.123,
	                      0.000000013143,
	                      0.0001123434,
	                      4533.5,
	                      1.0,
	                      1.000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      0.0000000000000000001,
	                      1.657348632812501,
	                      1032.03423423 };
	auto s_kahan = SumKahanBabushkaNeumaier( da );
	daw::println( "{:.19f}\nThe Kahan sum is {:.19f}",
	              daw::fmt_range{ da, "\n\t", "[\t", " ]" },
	              s_kahan );
	auto s_naive = Sum( da );
	daw::println( "The naive sum is {:.19f}", s_naive );

	daw::println( "{}", daw::fmt_range( va ) );
	daw::println( "Done" );
}
