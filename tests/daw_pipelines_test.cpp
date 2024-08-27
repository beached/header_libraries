// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_pipelines.h>

#include <daw/daw_constant.h>
#include <daw/daw_do_not_optimize.h>
#include <daw/daw_ensure.h>
#include <daw/daw_print.h>
#include <daw/daw_random.h>

#include <array>
#include <iterator>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace daw::pipelines;

struct AlwaysTrue_t {
	std::true_type constexpr operator( )( auto const &... ) {
		return { };
	}
};

static_assert( std::forward_iterator<filter_view<char const *, AlwaysTrue_t>> );

inline constexpr auto vowel =
  IsOneOf<'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U'>;

inline constexpr auto is_letter = []( char c ) {
	return ( 'A' <= c and c <= 'Z' ) or ( 'a' <= c and c <= 'z' );
};

inline constexpr auto to_lower = []( char c ) -> char {
	return static_cast<char>( static_cast<unsigned char>( c ) | ' ' );
};

namespace tests {
	static constexpr auto prices = std::array{ 100, 200, 150, 180, 130 };
	static constexpr auto costs = std::array{ 10, 20, 50, 40, 100 };

	DAW_ATTRIB_NOINLINE void test001( ) {
		auto m1 = pipeline( zip_view( prices, costs ), To<std::map> );
		daw_ensure( m1[prices[0]] == costs[0] );
		daw::println(
		  "\ntest001: pipeline( zip_view( prices, costs ), To<std::map> )\n{}",
		  daw::fmt_range{ m1 } );
	}

	struct ToLetter_t {
		DAW_CPP23_STATIC_CALL_OP constexpr char
		operator( )( auto i ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if( i < 26 ) {
				return static_cast<char>( 'a' + i );
			} else if( i < 52 ) {
				return static_cast<char>( 'A' + ( i - 26 ) );
			}
			throw std::out_of_range( "i is out of range" );
		}
	};
	inline constexpr auto to_letter = ToLetter_t{ };

	DAW_ATTRIB_NOINLINE void test002( ) {
		constexpr auto pm2 = pipeline( Map( to_letter ), Enumerate, To<std::map> );
		auto const m2 = pm2( iota_view( 0, 26 ) );
		daw::println(
		  "\ntest002: pipeline( Map( to_letter ), Enumerate, To<std::map> )\n{}",
		  daw::fmt_range( m2 ) );
	}

	DAW_ATTRIB_NOINLINE void test003( ) {

		constexpr auto q = pipeline( Filter( Not( vowel ) ),
		                             Take( 8 ),
		                             Map( to_lower ),
		                             Filter( is_letter ),
		                             To<std::basic_string>,
		                             Sort,
		                             Unique,
		                             Map( []( long long i ) {
			                             return i;
		                             } ),
		                             Sum );
		auto const out0 = q( "Hello World, How are you?" );
		auto const out1 = q( "Blah blah blah, how is yah?" );
		daw::println(
		  "\ntest003: pipeline(\n\tFilter( Not( vowel ) ),\n\tTake( 8 ),\n\tMap( "
		  "to_lower ),Filter( is_letter "
		  "),\n\tTo<std::basic_string>,\n\tSort,\n\tUnique,\n\tMap( []( long "
		  "long "
		  "i ) {{ return i; }}),\n\tSum );" );
		daw::println( "\tinput 'Hello World, How are you?' - output {}", out0 );
		daw::println( "\tinput 'Blah blah blah, how is yah?' - output {}", out1 );
	}

	DAW_ATTRIB_NOINLINE void test004( ) {
		constexpr auto p = pipeline( Filter( Not( vowel ) ),
		                             Print,
		                             Take( 8 ),
		                             Print,
		                             Map( to_lower ),
		                             Print,
		                             Filter( is_letter ) );
		constexpr auto p1 = pipeline( p, Take( 4 ) );
		daw::println(
		  "\ntest004: pipeline(Filter( Not( vowel ) ), Take( 8 ), Map( to_lower "
		  "), "
		  "Filter( is_letter ) )" );
		auto r0 = p1( "Hello World, How are you?" );
		auto r1 = p1( "Blah blah blah, how is yah?" );
		daw::println( "\tinput 'Hello World, How are you?': output {}",
		              daw::fmt_range( r0 ) );
		daw::println( "\tinput 'Blah blah blah, how is yah?': output {}",
		              daw::fmt_range( r1 ) );
	}

	DAW_ATTRIB_NOINLINE void test005( ) {
		// Example from https://youtu.be/itnyR9j8y6E?t=404
		constexpr auto p3 =
		  pipeline( Map( []( auto i ) {
			            return std::pair{ i, std::to_string( i ) };
		            } ),
		            Filter( []( auto const &v ) {
			            auto lhs = static_cast<std::size_t>( v.first );
			            auto const h = std::hash<std::string>{ }( v.second );
			            return lhs < h;
		            } ),
		            Map( &std::pair<std::size_t, std::string>::second ),
		            Take( 4 ),
		            To<std::array<std::string, 4>> );
		auto const result = p3( iota_view( 1, 10001 ) );
		daw::println( "\ntest005: input 1 to 10001 : output {}",
		              daw::fmt_range( result ) );
	}

	DAW_ATTRIB_NOINLINE void test006( ) {
		auto const r2 =
		  pipeline( iota_view( 1, 10001 ),
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
		daw::println( "\ntest006: input 1 to 10001 : output {}",
		              daw::fmt_range( r2 ) );
	}

	consteval void test007( ) {
		constexpr auto p4 = pipeline( Filter( []( int x ) {
			                              return x % 2 == 0;
		                              } ),
		                              Map( []( std::optional<int> x ) {
			                              if( not x ) {
				                              return iota_view<int>{ 1 };
			                              }
			                              return iota_view<int>{ *x };
		                              } ),
		                              Count,
		                              ToIota,
		                              Map( []( int x ) {
			                              return x * 2;
		                              } ) );
		constexpr auto x = p4( std::array{ 1, 2, 3 } );
		constexpr auto y = *std::next( x.begin( ) );
		static_assert( y == 2 );
	}

	DAW_ATTRIB_NOINLINE void test008( ) {
		daw::println(
		  "\ntest008: pipeline(\n\t\tMap( [](unsigned i) {{\n\t\t\treturn "
		  "std::tuple(i,to_letter(i));\n\t\t}} "
		  "),\n\t\tPrint,\n\t\tSwizzle<1,0,0>,\n\t\tPrint\n\t)\n\tinput 0 to 52: "
		  "output\n" );
		constexpr auto tp0 = pipeline( Map( []( unsigned i ) {
			                               return std::tuple( i, to_letter( i ) );
		                               } ),
		                               Print,
		                               Swizzle<1, 0, 0>,
		                               Print );
		(void)tp0( iota_view<unsigned>( 0, 52 ) );
	}

	DAW_ATTRIB_NOINLINE void test009( ) {
		daw::println(
		  "\ntest009: pipeline( ZipMore( iota_view<char>( 'A', 'Z' ) ) )" );
		constexpr auto tp1 = pipeline( ZipMore( iota_view<char>( 'A', 'Z' ) ) );

		constexpr auto tp1r0 = tp1( iota_view<char>( 'a', 'z' ) );
		daw::println( "\n\tinput: iota_view<char>( 'a', 'z' ): output\n{}",
		              daw::fmt_range( tp1r0 ) );

		constexpr auto tp1r1 = tp1(
		  zip_view( iota_view<char>( 'a', 'z' ), iota_view<char>( 'a', 'z' ) ) );
		daw::println(
		  "\n\tinput:zip_view( iota_view<char>( 'a', 'z' ), iota_view<char>( "
		  "'a', "
		  "'z' ) ) : output\n{}",
		  daw::fmt_range( tp1r1 ) );
	}

	DAW_ATTRIB_NOINLINE void fma( std::size_t N,
	                              double const *__restrict a,
	                              double const *__restrict b,
	                              double const *__restrict c,
	                              double *__restrict out ) {
		auto const fma_range =
		  pipeline( zip_view( daw::contiguous_view( a, N ),
		                      daw::contiguous_view( b, N ),
		                      daw::contiguous_view( c, N ) ),
		            MapApply( []( double x, double y, double z ) {
			            auto result = x + y * z;
			            return result;
		            } ) );
		static_assert( std::same_as<daw::range_category_t<DAW_TYPEOF( fma_range )>,
		                            std::random_access_iterator_tag> );
		auto first = std::begin( fma_range );
		auto const last = std::end( fma_range );
		while( first != last ) {
			*out = *first;
			++out;
			++first;
		}
	}

	DAW_ATTRIB_NOINLINE void test010( ) {
		daw::println( "\ntest010: fma test" );
		auto const make_random =
		  pipeline( Map( [rnd = daw::RandomFloat<double>{ }]( auto ) mutable {
			            auto result = rnd( );
			            return result;
		            } ),
		            To<std::vector> );
		std::vector<double> const a = make_random( iota_view( 16 ) );
		daw::println( "\na: {}", daw::fmt_range( a ) );
		daw::do_not_optimize( a );
		std::vector<double> const b = make_random( iota_view( 16 ) );
		daw::println( "\nb: {}", daw::fmt_range( b ) );
		daw::do_not_optimize( b );
		std::vector<double> const c = make_random( iota_view( 16 ) );
		daw::println( "\nc: {}", daw::fmt_range( c ) );
		daw::do_not_optimize( c );
		auto o = std::vector<double>( 16ULL );
		daw::do_not_optimize( o );
		fma( o.size( ),
		     std::data( a ),
		     std::data( b ),
		     std::data( c ),
		     std::data( o ) );
		daw::println( "\no: {}", daw::fmt_range( o ) );
		daw::do_not_optimize( o );
		daw_ensure( a[0] != o[0] );
	}

	DAW_ATTRIB_NOINLINE void test011( ) {
		constexpr auto a =
		  std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		constexpr auto p0 = pipeline( Skip( 1 ), Take( 5 ), To<std::vector> );
		auto v0 = p0( a );
		daw::println(
		  "\ntest011: pipeline( a, Skip( 1 ), Take( 5 ), To<std::vector> "
		  "):\n\tinput: "
		  "[1,2,3,4,5,6,7,8,9]\n\touput: {}",
		  daw::fmt_range( v0 ) );
	}

	DAW_ATTRIB_INLINE void test012( ) {
		auto v = pipeline( iota_view<int>( 1, 101 ), To<std::array<int, 100>> );
		daw::do_not_optimize( v );
		auto s = Sum( v );
		daw::println(
		  "\ntest012: pipeline( iota_view<int>( 1, 101 ), To<std::array<int, 100>> "
		  ")\nSum: {} of\n{}",
		  s,
		  daw::fmt_range( v ) );
	}

	DAW_ATTRIB_NOINLINE void test013( ) {
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
		                  daw::constant<ArraySize>, C const &c ) {
			using value_t = typename C::value_type;
			return To<std::array<value_t, ArraySize>>( )( c );
		};

		constexpr auto pm = pipeline( Map( []( int i ) {
			                              return std::pair{ i, to_letter( i ) };
		                              } ),
		                              To<std::map> );
		auto const m = pm( iota_view<int>( 0, 26 ) );
		daw::println( "To<std::map>\n{}", daw::fmt_range( m ) );
		auto va = to_array( daw::constant<std::size_t{ 15 }>{ }, v );
		daw::println( "{}", daw::fmt_range( va ) );
	}

	void test014( ) {
		daw::println( "\ntest014 - naive vs Kahan FP summation" );
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
		              daw::fmt_range{ da, ",\n\t", "[\t", " ]" },
		              s_kahan );
		auto s_naive = Sum( da );
		daw::println( "The naive sum is {:.19f}", s_naive );
	}

	DAW_ATTRIB_NOINLINE void test015( ) {
		constexpr auto a = pipeline( generate_n( 5,
		                                         [i = 1]( ) mutable {
			                                         return i++;
		                                         } ),
		                             To<std::array<std::size_t, 5>> );
		daw::println(
		  "\ntest015: pipeline( generate_n( 5, [&]{{return "
		  "i++;}}),To<std::array<std::size_t, 5>>);\n{}",
		  daw::fmt_range{ a } );
	}

	DAW_ATTRIB_NOINLINE void test016( ) {
		constexpr auto a =
		  std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		constexpr auto p0 = pipeline( Skip( 3 ), To<std::vector> );
		auto v0 = p0( a );
		daw::println(
		  "\ntest016: pipeline( a, Skip( 3 ), To<std::vector> ):\n\tinput: "
		  "[1,2,3,4,5,6,7,8,9]\n\touput: {}",
		  daw::fmt_range( v0 ) );
	}

	DAW_ATTRIB_NOINLINE void test017( ) {
		daw::println(
		  "\ntest017: pipeline( iota_view<int>{{ -5, 5 }}, Sample( 4, 10 ) )" );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, Print, Sample( 4, 10 ) );
		daw::println( "{}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test018( ) {
		daw::println( "\ntest018: pipeline( iota_view<int>{{ -5, 5 }}, Reverse )" );
		auto i = iota_view<int>{ -5, 5 };
		daw::println( "\t{}", daw::fmt_range( i ) );
		daw::println( "\treversed" );
		daw::println( "\t{}", daw::fmt_range( Reverse( i ) ) );
	}

	DAW_ATTRIB_NOINLINE void test019( ) {
		daw::println(
		  "\ntest019: pipeline( iota_view<int>{{ -5, 5 }}, Every( 3 ) )" );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, Print, Every( 3 ) );
		daw::println( "{}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test020( ) {
		daw::println(
		  "\ntest020: pipeline( iota_view<int>{{ -5, 5 }}, TakeUntil( []( int x ) "
		  "{{ return x > 0; }} ) ) )" );
		daw::println( "\tin:  {}", daw::fmt_range{ iota_view<int>{ -5, 5 } } );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, TakeUntil( []( int x ) {
			                         return x > 0;
		                         } ) );
		daw::println( "\tout: {}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test021( ) {
		daw::println(
		  "\ntest021: pipeline( iota_view<int>{{ -5, 5 }}, TakeWhile( []( int x ) "
		  "{{ return x < 0; }} ) ) )" );
		daw::println( "\tin:  {}", daw::fmt_range{ iota_view<int>{ -5, 5 } } );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, TakeWhile( []( int x ) {
			                         return x < 0;
		                         } ) );
		daw::println( "\tout: {}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test022( ) {
		daw::println( "\ntest022: pipeline( Print, Flatten, Print )" );
		constexpr auto ary = std::array{ iota_view<int>{ 1, 2 },
		                                 iota_view<int>{ 6, 10 },
		                                 iota_view<int>{ 15, 17 } };
		auto r = pipeline( ary, Print, Flatten, Print );
		(void)r;
	}

	DAW_ATTRIB_NOINLINE void test023( ) {
		static constexpr auto ary =
		  To<std::array<std::size_t, 9>>( )( iota_view( 9 ) );
		daw::println( "\ntest023: pipeline( Chunk( 3 ), Flatten )" );
		daw::println( "\tin:  {}", daw::fmt_range( ary ) );
		constexpr auto p = pipeline( Chunk( 3 ), Print, Flatten );
		auto r = p( ary );
		auto const ary2 = To<std::array<std::size_t, 9>>( )( r );
		daw_ensure( ary == ary2 );
		daw::println( "\tout: {}", daw::fmt_range( ary2 ) );
	}
} // namespace tests

int main( ) {
	tests::test001( );
	tests::test002( );
	tests::test003( );
	tests::test004( );
	tests::test005( );
	tests::test006( );
	tests::test007( );
	tests::test008( );
	tests::test009( );
	tests::test010( );
	tests::test011( );
	tests::test012( );
	tests::test013( );
	tests::test014( );
	tests::test015( );
	tests::test016( );
	tests::test017( );
	tests::test018( );
	tests::test019( );
	tests::test020( );
	tests::test021( );
	tests::test022( );
	tests::test023( );

	daw::println( "Done" );
}
