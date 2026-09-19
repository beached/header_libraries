// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_pipelines.h>

#include <daw/daw_constant.h>
#include <daw/daw_contiguous_view.h>
#include <daw/daw_do_not_optimize.h>
#include <daw/daw_ensure.h>
#include <daw/daw_formatters.h>
#include <daw/daw_print.h>
#include <daw/daw_random.h>
#include <daw/daw_string_view.h>

#include <array>
#include <forward_list>
#include <iterator>
#include <map>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace daw::pipelines;

using namespace daw::string_view_literals;
struct AlwaysTrue_t {
	std::true_type constexpr operator( )( auto const &... ) {
		return { };
	}
};

static_assert(
  daw::ForwardRange<filter_view<daw::view<char const *>, AlwaysTrue_t>> );

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

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void test_zip_view_to_map_matches_source_pairs( ) {
		auto m1 = pipeline( zip_view( prices, costs ), To<std::map> );
		daw_ensure( m1.size( ) == prices.size( ) );
		for( std::size_t n = 0; n < prices.size( ); ++n ) {
			daw_ensure( m1[prices[n]] == costs[n] );
		}
	}
#endif

	struct ToLetter_t {
		DAW_CPP23_STATIC_CALL_OP constexpr char
		operator( )( auto i ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if( i < 26 ) {
				return static_cast<char>( 'a' + i );
			} else if( i < 52 ) {
				return static_cast<char>( 'A' + ( i - 26 ) );
			}
			daw::println( stderr,
			              "{}:{} is out of range",
			              static_cast<char>( i ),
			              static_cast<int>( i ) );
			std::terminate( );
		}
	};
	inline constexpr auto to_letter = ToLetter_t{ };

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void test_enumerate_and_enumerate_with_map_to_letters( ) {
		constexpr auto pm2 = pipeline( Map( to_letter ), Enumerate, To<std::map> );
		auto const m2 = pm2( iota_view( 0, 26 ) );
		for( auto const &[k, v] : m2 ) {
			daw_ensure( v == static_cast<char>( 'a' + k ) );
		}

		constexpr auto pm3 =
		  pipeline( Map( to_letter ), EnumerateWith<int>, To<std::map> );
		auto const m3 = pm3( iota_view( 0, 26 ) );
		for( auto const &[k, v] : m3 ) {
			daw_ensure( v == static_cast<char>( 'a' + k ) );
		}

		for( auto vec = std::vector{ 1, 2, 3 };
		     auto [index, value] : EnumerateWith<int>( vec ) ) {
			daw::dump( index * value );
		}
	}
#endif

	DAW_ATTRIB_NOINLINE void test_string_pipeline_filter_sort_unique_sum( ) {

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
		  "\ntest_string_pipeline_filter_sort_unique_sum: pipeline(\n\tFilter( "
		  "Not( vowel ) ),\n\tTake( 8 ),\n\tMap( "
		  "to_lower ),Filter( is_letter "
		  "),\n\tTo<std::basic_string>,\n\tSort,\n\tUnique,\n\tMap( []( long "
		  "long "
		  "i ) {{ return i; }}),\n\tSum );" );
		daw::println( "\tinput 'Hello World, How are you?' - output {}", out0 );
		daw::println( "\tinput 'Blah blah blah, how is yah?' - output {}", out1 );
	}

	DAW_ATTRIB_NOINLINE void
	test_pipeline_composed_as_stage_of_another_pipeline( ) {
		constexpr auto p = pipeline( Filter( Not( vowel ) ),
		                             Print,
		                             Take( 8 ),
		                             Print,
		                             Map( to_lower ),
		                             Print,
		                             Filter( is_letter ) );
		constexpr auto p1 = pipeline( p, Take( 4 ) );
		daw::println(
		  "\ntest_pipeline_composed_as_stage_of_another_pipeline: pipeline(Filter( "
		  "Not( vowel ) ), Take( 8 ), Map( to_lower "
		  "), "
		  "Filter( is_letter ) )" );
		auto r0 = p1( "Hello World, How are you?" );
		auto r1 = p1( "Blah blah blah, how is yah?" );
		daw::println( "\tinput 'Hello World, How are you?': output {}",
		              daw::fmt_range( r0 ) );
		daw::println( "\tinput 'Blah blah blah, how is yah?': output {}",
		              daw::fmt_range( r1 ) );
	}

	DAW_ATTRIB_NOINLINE void test_pipeline_callable_reused_across_inputs( ) {
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
		daw::println(
		  "\ntest_pipeline_callable_reused_across_inputs: input 1 to 10001 : "
		  "output {}",
		  daw::fmt_range( result ) );
	}

	DAW_ATTRIB_NOINLINE void test_pipeline_with_source_baked_in( ) {
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
		daw::println(
		  "\ntest_pipeline_with_source_baked_in: input 1 to 10001 : output {}",
		  daw::fmt_range( r2 ) );
	}

	consteval void test_consteval_filter_map_optional_count_to_iota( ) {
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
		auto x = p4( std::array{ 1, 2, 3 } );
		auto y = *std::next( x.begin( ) );
		daw_ensure( y == 2 );
	}

	DAW_ATTRIB_NOINLINE void test_swizzle_tuple_elements( ) {
		daw::println(
		  "\ntest_swizzle_tuple_elements: pipeline(\n\t\tMap( [](unsigned i) "
		  "{{\n\t\t\treturn "
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

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void test_zip_more_appends_range_to_view_and_zip( ) {
		daw::println(
		  "\ntest_zip_more_appends_range_to_view_and_zip: pipeline( ZipMore( "
		  "iota_view<char>( 'A', 'Z' ) ) )" );
		static constexpr auto tp1 =
		  pipeline( ZipMore( iota_view<char>( 'A', 'Z' ) ) );

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

	DAW_ATTRIB_NOINLINE void fma( std::size_t N, double const *__restrict a,
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

	DAW_ATTRIB_NOINLINE void test_fma_pipeline_matches_manual_computation( ) {
		daw::println( "\ntest_fma_pipeline_matches_manual_computation: fma test" );
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
#endif

	DAW_ATTRIB_NOINLINE void test_skip_then_take( ) {
		constexpr auto a =
		  std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		constexpr auto p0 = pipeline( Skip( 1 ), Take( 5 ), To<std::vector> );
		auto v0 = p0( a );
		daw::println(
		  "\ntest_skip_then_take: pipeline( a, Skip( 1 ), Take( 5 ), "
		  "To<std::vector> "
		  "):\n\tinput: "
		  "[1,2,3,4,5,6,7,8,9]\n\touput: {}",
		  daw::fmt_range( v0 ) );
	}

	DAW_ATTRIB_INLINE void test_iota_to_fixed_array_and_sum( ) {
		auto v = pipeline( iota_view<int>( 1, 101 ), To<std::array<int, 100>> );
		daw::do_not_optimize( v );
		auto s = Sum( v );
		daw::println(
		  "\ntest_iota_to_fixed_array_and_sum: pipeline( iota_view<int>( 1, 101 ), "
		  "To<std::array<int, 100>> "
		  ")\nSum: {} of\n{}",
		  s,
		  daw::fmt_range( v ) );
	}

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void
	test_pipeline_mapapply_clamp_concat_and_conversions( ) {
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
#endif

	void test_kahan_vs_naive_summation( ) {
		daw::println(
		  "\ntest_kahan_vs_naive_summation - naive vs Kahan FP summation" );
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

	DAW_ATTRIB_NOINLINE void test_generate_n_to_fixed_array( ) {
		constexpr auto a = pipeline( generate_n( 5,
		                                         [i = 1]( ) mutable {
			                                         return i++;
		                                         } ),
		                             To<std::array<std::size_t, 5>> );
		daw::println(
		  "\ntest_generate_n_to_fixed_array: pipeline( generate_n( 5, [&]{{return "
		  "i++;}}),To<std::array<std::size_t, 5>>);\n{}",
		  daw::fmt_range{ a } );
	}

	DAW_ATTRIB_NOINLINE void test_skip_elements( ) {
		constexpr auto a =
		  std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		constexpr auto p0 = pipeline( Skip( 3 ), To<std::vector> );
		auto v0 = p0( a );
		daw::println(
		  "\ntest_skip_elements: pipeline( a, Skip( 3 ), To<std::vector> "
		  "):\n\tinput: "
		  "[1,2,3,4,5,6,7,8,9]\n\touput: {}",
		  daw::fmt_range( v0 ) );
	}

	DAW_ATTRIB_NOINLINE void test_sample_from_iota_view( ) {
		daw::println(
		  "\ntest_sample_from_iota_view: pipeline( iota_view<int>{{ -5, 5 }}, "
		  "Sample( 4, 10 ) )" );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, Print, Sample( 4, 10 ) );
		daw::println( "{}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test_reverse_iota_view( ) {
		daw::println(
		  "\ntest_reverse_iota_view: pipeline( iota_view<int>{{ -5, 5 }}, Reverse "
		  ")" );
		auto i = iota_view<int>{ -5, 5 };
		daw::println( "\t{}", daw::fmt_range( i ) );
		daw::println( "\treversed" );
		daw::println( "\t{}", daw::fmt_range( ReverseView( i ) ) );
	}

	DAW_ATTRIB_NOINLINE void test_every_nth_element( ) {
		daw::println(
		  "\ntest_every_nth_element: pipeline( iota_view<int>{{ -5, 5 }}, Every( 3 "
		  ") )" );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, Print, Every( 3 ) );
		daw::println( "{}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test_take_until_predicate( ) {
		daw::println(
		  "\ntest_take_until_predicate: pipeline( iota_view<int>{{ -5, 5 }}, "
		  "TakeUntil( []( int x ) "
		  "{{ return x > 0; }} ) ) )" );
		daw::println( "\tin:  {}", daw::fmt_range{ iota_view<int>{ -5, 5 } } );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, TakeUntil( []( int x ) {
			                         return x > 0;
		                         } ) );
		daw::println( "\tout: {}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test_take_while_predicate( ) {
		daw::println(
		  "\ntest_take_while_predicate: pipeline( iota_view<int>{{ -5, 5 }}, "
		  "TakeWhile( []( int x ) "
		  "{{ return x < 0; }} ) ) )" );
		daw::println( "\tin:  {}", daw::fmt_range{ iota_view<int>{ -5, 5 } } );
		auto const p = pipeline( iota_view<int>{ -5, 5 }, TakeWhile( []( int x ) {
			                         return x < 0;
		                         } ) );
		daw::println( "\tout: {}", daw::fmt_range{ p } );
	}

	DAW_ATTRIB_NOINLINE void test_flatten_nested_iota_views( ) {
		daw::println(
		  "\ntest_flatten_nested_iota_views: pipeline( Print, Flatten, Print )" );
		constexpr auto ary = std::array{ iota_view<int>{ 1, 2 },
		                                 iota_view<int>{ 6, 10 },
		                                 iota_view<int>{ 15, 17 } };
		auto r = pipeline( ary, Print, Flatten, Print );
		(void)r;
	}

	DAW_ATTRIB_NOINLINE void test_chunk_then_flatten_round_trip( ) {
		static constexpr auto ary =
		  To<std::array<std::size_t, 9>>( )( iota_view( 9 ) );
		daw::println(
		  "\ntest_chunk_then_flatten_round_trip: pipeline( Chunk( 3 ), Flatten )" );
		daw::println( "\tin:  {}", daw::fmt_range( ary ) );
		constexpr auto p = pipeline( Chunk( 3 ), Print, Flatten );
		auto r = p( ary );
		auto const ary2 = To<std::array<std::size_t, 9>>( )( r );
		daw_ensure( ary == ary2 );
		daw::println( "\tout: {}", daw::fmt_range( ary2 ) );
	}

	DAW_ATTRIB_NOINLINE void test_find_if_and_find_on_array( ) {
		static constexpr auto ary = std::array{ 1, 2, 3, 4, 4, 5, 6, 6, 7 };
		constexpr auto it = FindIf<2>( ary, []( auto const &l, auto const &r ) {
			return l == r;
		} );
		static_assert( it != std::end( ary ) );
		static_assert( std::distance( std::begin( ary ), it ) == 3 );

		constexpr auto it2 = Find( ary, 2 );
		static_assert( it2 != std::end( ary ) );
		static_assert( std::distance( std::begin( ary ), it2 ) == 1 );
		constexpr auto it3 = Find( 2 )( ary );
		static_assert( it2 == it3 );
	}

	DAW_ATTRIB_NOINLINE void test_find_with_member_projection( ) {
		struct Test {
			int value;

			explicit( false ) constexpr Test( int v )
			  : value( v ) {}
		};
		static constexpr auto ary =
		  std::array<Test, 9>{ 1, 2, 3, 4, 4, 5, 6, 6, 7 };
		constexpr auto it = FindIf<2>( ary, []( auto const &l, auto const &r ) {
			return l.value == r.value;
		} );
		static_assert( it != std::end( ary ) );
		static_assert( std::distance( std::begin( ary ), it ) == 3 );

		constexpr auto it2 = Find( ary, 2, &Test::value );
		static_assert( it2 != std::end( ary ) );
		static_assert( std::distance( std::begin( ary ), it2 ) == 1 );
		constexpr auto it3 = Find( 2, &Test::value )( ary );
		static_assert( it2 == it3 );
	}

	DAW_ATTRIB_NOINLINE void test_slide_then_flatten( ) {
		static constexpr auto ary =
		  To<std::array<std::size_t, 9>>( )( iota_view( 9 ) );
		daw::println(
		  "\ntest_slide_then_flatten: pipeline( Slide( 3 ), Flatten )" );
		daw::println( "\tin:  {}", daw::fmt_range( ary ) );
		constexpr auto p = pipeline( Slide( 3 ), Print, Flatten );
		auto r = p( ary );
		auto const ary2 = To<std::array<std::size_t, 9>>( )( r );
		constexpr auto tst_ary =
		  std::array<std::size_t, 9>{ 0, 1, 2, 1, 2, 3, 2, 3, 4 };
		daw_ensure( ary2 == tst_ary );
		daw::println( "\tout: {}", daw::fmt_range( ary2 ) );
	}

	DAW_ATTRIB_NOINLINE void test_slide_window_starts( ) {
		auto const starts = pipeline( std::array{ 1, 2, 3, 4 },
		                              Slide( 2 ),
		                              Map( []( auto window ) {
			                              return *window.begin( );
		                              } ),
		                              To<std::vector> );
		daw_ensure( starts == std::vector{ 1, 2, 3 } );
	}

	DAW_ATTRIB_NOINLINE void test_contains_string_view_array( ) {
		static constexpr std::array ids = { "597216680271282192"_sv,
		                                    "155149108183695360"_sv,
		                                    "843841057833877588"_sv,
		                                    "1013956772245020774"_sv,
		                                    "1013960757127422113"_sv };
		static constexpr auto exists = []( daw::string_view needle ) {
			return Contains( needle )( ids );
		};
		constexpr auto x = "12345"_sv;
		static_assert( not exists( x ) );
		constexpr auto y = "1013960757127422113"_sv;
		static_assert( exists( y ) );
	}

	DAW_ATTRIB_NOINLINE void test_dump_smoke_test( ) {
		daw::dump( 1, 2, 3, 4 );
	}

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void test_enumerate_from_offset( ) {
		constexpr auto pm3 =
		  pipeline( Map( to_letter ), EnumerateFrom( 2 ), To<std::map> );
		auto const m3 = pm3( iota_view( 0, 26 ) );
		daw::println(
		  "\ntest_enumerate_from_offset: pipeline( Map( to_letter ), "
		  "EnumerateFrom( 2 ), "
		  "To<std::map> "
		  ")\n{}",
		  daw::fmt_range( m3 ) );

		for( auto vec = std::vector{ 1, 2, 3 };
		     auto [index, value] : EnumerateFrom( 2 )( vec ) ) {
			daw::dump( index * value );
		}
	}
#endif

	DAW_ATTRIB_NOINLINE void test_split_by_delimiter_and_trim( ) {
		static constexpr auto comma_splitter =
		  pipeline( Split( ',' ), Map( []( daw::Range auto r ) {
			            return daw::string_view( r ).trim( );
		            } ) );
		static constexpr auto values = "1a, 2b,3c, 4d ,5e"_sv;

		constexpr auto parts = comma_splitter( values );
		daw::println(
		  "test_split_by_delimiter_and_trim: pipeline( Split( ',' ), \"1a, 2b, 3c, "
		  "4d, 5e\" )\n{}",
		  daw::fmt_range{ parts, "|" } );
	}

	DAW_ATTRIB_NOINLINE void test_split_by_subsequence_delimiter( ) {
		static constexpr auto nums_to_string =
		  pipeline( Map( []( int n ) {
			            assert( n >= 0 and n <= 9 );
			            return static_cast<char>( n + '0' );
		            } ),
		            []( auto r ) {
			            return std::format( "\"{}\"", daw::fmt_range( r, "" ) );
		            } );

		static constexpr auto delim = std::array{ 3, 4 };
		constexpr auto splitter = pipeline( Split( delim ), Map( nums_to_string ) );
		static constexpr auto data =
		  std::array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 42, 42, 42, 42, 42, 42 };

		static constexpr auto parts =
		  splitter( daw::contiguous_view( data.data( ), 9U ) );

		daw::println(
		  "test_split_by_subsequence_delimiter: pipeline( Split( {{3,4}} ), Map( "
		  "nums_to_string ) ) with [1, "
		  "2, 3, 4, 5, 6, 7, 8, 9]" );

		daw::println( "{}", daw::fmt_range( parts ) );
	}

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void test_curried_zip_stage_in_pipeline( ) {
		static constexpr int arr1[] = { 0, 1, 2, 3, 4 };
		static constexpr int arr2[] = { 0, 100, 200, 300, 400 };

		constexpr auto zipped = pipeline( Zip, MapApply( std::plus<>{ } ) );
		daw::println(
		  "test_curried_zip_stage_in_pipeline: Expecting: 0, 101, 202, 303, 404" );
		for( auto p : zipped( arr1, arr2 ) ) {
			daw::println( "{}", p );
		}
		constexpr auto zipped2 =
		  pipeline( Zip( arr1, arr2 ), MapApply( std::plus<>{ } ) );
		for( auto p : zipped2 ) {
			daw::println( "{}", p );
		}

		for( auto p : pipeline( Zip, MapApply( []( int x ) {
			                        return x + 1;
		                        } ) )( arr1 ) ) {
			daw::println( "{}", p );
		}
	}
#endif

	constexpr bool is_control_or_space( const char c ) noexcept {
		return static_cast<unsigned char>( c ) <= static_cast<unsigned char>( ' ' );
	}

	DAW_ATTRIB_NOINLINE void test_trim_both_ends_with_drop_while_reverse( ) {
		static constexpr std::string_view s = " Hello ";
		static constexpr auto p = pipeline( s,
		                                    DropWhile( is_control_or_space ),
		                                    ReverseView,
		                                    DropWhile( is_control_or_space ),
		                                    ReverseView,
		                                    To<std::array<char, 6>> );
		constexpr auto s2 = daw::string_view{ std::data( p ), std::size( p ) - 1 };
		daw_ensure( s2 == "Hello" );
	}

	DAW_ATTRIB_NOINLINE void test_trimmed_first_ref_returns_first_char( ) {
		constexpr daw::string_view s = " Hello ";
		constexpr auto p = pipeline( s,
		                             DropWhile( is_control_or_space ),
		                             ReverseView,
		                             DropWhile( is_control_or_space ),
		                             ReverseView,
		                             FirstRef );
		daw_ensure( p.has_value( ) and p.value( ) == 'H' );
	}

	DAW_ATTRIB_NOINLINE void test_first_ref_reflects_mutation( ) {
		char buff[] = "Hello";
		auto p = pipeline( buff, FirstRef );
		daw_ensure( p.has_value( ) and p.value( ) == 'H' );
		buff[0] = 'h';
		daw_ensure( p.value( ) == 'h' );
	}

	DAW_ATTRIB_NOINLINE void test_trimmed_first_returns_first_char( ) {
		constexpr daw::string_view s = " Hello ";
		constexpr auto p = pipeline( s,
		                             DropWhile( is_control_or_space ),
		                             ReverseView,
		                             DropWhile( is_control_or_space ),
		                             ReverseView,
		                             First );
		daw_ensure( p.has_value( ) and p.value( ) == 'H' );
	}

	DAW_ATTRIB_NOINLINE DAW_CONSTEVAL void
	test_first_value_does_not_reflect_mutation( ) {
		char buff[] = "Hello";
		auto p = pipeline( buff, First );
		daw_ensure( p.has_value( ) and p.value( ) == 'H' );
		buff[0] = 'h';
		daw_ensure( p.value( ) == 'H' );
	}

	DAW_ATTRIB_NOINLINE void test_copy_sort_unique_pipeline( ) {
		constexpr auto values = std::array{ 1, 5, 5, 10, 32 };
		daw::do_not_optimize( values );
		auto const unique_values =
		  pipeline( values, Copy, Sort, Unique, To<std::vector> );
		daw_ensure( unique_values.size( ) == 4 );
		daw_ensure( unique_values == std::vector{ 1, 5, 10, 32 } );
	}

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void test_elements_projection_returns_tuples( ) {
		auto z0 = Zip( prices, costs );

		auto e0 = Elements<1>( z0 );
		daw_ensure( static_cast<std::size_t>( std::ranges::distance( e0 ) ) ==
		            costs.size( ) );
		auto eb0 = std::begin( e0 );
		for( std::size_t n = 0; n < costs.size( ); ++n ) {
			daw_ensure( std::get<0>( eb0[n] ) == costs[n] );
		}
		auto e1 = Elements<0>( z0 );
		auto eb1 = std::begin( e1 );
		daw_ensure( static_cast<std::size_t>( std::ranges::distance( e1 ) ) ==
		            prices.size( ) );
		for( std::size_t n = 0; n < prices.size( ); ++n ) {
			daw_ensure( std::get<0>( eb1[n] ) == prices[n] );
		}
	}

	DAW_ATTRIB_NOINLINE void test_element_projection_returns_values( ) {
		auto z0 = Zip( prices, costs );

		auto e0 = Element<1>( z0 );
		daw_ensure( static_cast<std::size_t>( std::ranges::distance( e0 ) ) ==
		            costs.size( ) );
		auto eb0 = std::begin( e0 );
		for( std::size_t n = 0; n < costs.size( ); ++n ) {
			daw_ensure( eb0[n] == costs[n] );
		}
		auto e1 = Element<0>( z0 );
		auto eb1 = std::begin( e1 );
		daw_ensure( static_cast<std::size_t>( std::ranges::distance( e1 ) ) ==
		            prices.size( ) );
		for( std::size_t n = 0; n < prices.size( ); ++n ) {
			daw_ensure( eb1[n] == prices[n] );
		}
	}
#endif

	DAW_ATTRIB_NOINLINE void test_cache_last_avoids_recomputation( ) {
		constexpr auto values = std::array{ 1, 5, 5, 10, 32 };
		auto cp = pipeline( values,
		                    Map( []( int x ) {
			                    static int map_count = 0;
			                    ++map_count;
			                    daw::println( "MapCount = {}", map_count );
			                    return x * x;
		                    } ),
		                    CacheLast );
		auto first = std::begin( cp );
		auto last = std::end( cp );
		while( first != last ) {
			daw::println( "{}", *first );
			daw::println( "{}", *first );
			++first;
		}
	}

	DAW_ATTRIB_NOINLINE void test_map_function_pointer_projection( ) {
		auto v = std::vector<std::string>{ "Hello", "World" };

		auto mapper = Map( +[]( std::string &s ) -> char * {
			return s.data( );
		} );
		auto m = mapper( v );
		auto v2 = std::vector<char *>{ };
		auto first = std::begin( m );
		auto const last = std::end( m );
		while( first != last ) {
			v2.push_back( *first );
			++first;
		}
		daw_ensure( v2.size( ) == v.size( ) );
	}

	DAW_ATTRIB_NOINLINE void test_take_larger_than_forward_range( ) {
		auto source = std::forward_list{ 1, 2, 3 };
		auto result = pipeline( source, Take( 10 ), To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3 } );
	}

	DAW_ATTRIB_NOINLINE void test_take_from_empty_forward_range( ) {
		auto empty =
		  pipeline( std::forward_list<int>{ }, Take( 10 ), To<std::vector> );
		daw_ensure( empty.empty( ) );
	}

	DAW_ATTRIB_NOINLINE void test_take_zero_from_forward_range( ) {
		auto source = std::forward_list{ 1, 2, 3 };
		auto zero = pipeline( source, Take( 0 ), To<std::vector> );
		daw_ensure( zero.empty( ) );
	}

	struct owned_value {
		int value;
	};

	DAW_ATTRIB_NOINLINE void test_map_rvalue_projection_dereferences_as_value( ) {
		auto source = std::array{ 1, 2, 3 };
		auto mapped = pipeline( source,
		                        Map( []( int value ) {
			                        return owned_value{ value };
		                        } ),
		                        Map( []( owned_value &&value ) -> int && {
			                        return std::move( value.value );
		                        } ) );

		using iterator = decltype( std::begin( mapped ) );
		static_assert(
		  std::is_same_v<decltype( *std::declval<iterator &>( ) ), int> );
	}

	DAW_ATTRIB_NOINLINE void test_map_materializes_projected_prvalues( ) {
		auto source = std::array{ 1, 2, 3 };
		auto mapped = pipeline( source,
		                        Map( []( int value ) {
			                        return owned_value{ value };
		                        } ),
		                        Map( []( owned_value &&value ) -> int && {
			                        return std::move( value.value );
		                        } ) );
		auto result = pipeline( mapped, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3 } );
	}

	DAW_ATTRIB_NOINLINE void test_const_map_uses_const_iterator( ) {
		auto const const_mapped =
		  pipeline( std::vector{ 1, 2, 3 }, Map( []( int value ) {
			            return value;
		            } ) );
		using const_iterator = decltype( std::begin( const_mapped ) );
		static_assert(
		  std::is_same_v<std::remove_cvref_t<
		                   decltype( std::declval<const_iterator &>( ).base( ) )>,
		                 std::vector<int>::const_iterator> );
	}

	DAW_ATTRIB_NOINLINE void test_const_map_can_be_materialized( ) {
		auto const const_mapped =
		  pipeline( std::vector{ 1, 2, 3 }, Map( []( int value ) {
			            return value;
		            } ) );
		auto const_result = pipeline( const_mapped, To<std::vector> );
		daw_ensure( const_result == std::vector{ 1, 2, 3 } );
	}

	DAW_ATTRIB_NOINLINE void test_map_after_const_unique_can_be_summed( ) {
		auto const const_unique_mapped =
		  pipeline( std::string{ "aabbcc" }, Unique, Map( []( char value ) {
			            return static_cast<int>( value );
		            } ) );
		auto const unique_sum = pipeline( const_unique_mapped, Sum );
		daw_ensure( unique_sum == 'a' + 'b' + 'c' );
	}

#if DAW_CPP_VERSION > 202002L
	template<typename Zipped>
	void verify_appended_ranges( Zipped const &zipped ) {
		using reference = decltype( *std::begin( zipped ) );
		using value = daw::remove_cvref_t<reference>;
		static_assert( std::tuple_size_v<value> == 3 );
		if constexpr( std::tuple_size_v<value> == 3 ) {
			auto first = std::begin( zipped );
			daw_ensure( std::get<0>( *first ) == 1 );
			daw_ensure( std::get<1>( *first ) == 3 );
			daw_ensure( std::get<2>( *first ) == 5 );
		}
	}

	DAW_ATTRIB_NOINLINE void test_zip_more_appends_to_zip( ) {
		auto const appended_to_zip = ZipMore( std::array{ 1, 2 } )(
		  Zip( std::array{ 3, 4 }, std::array{ 5, 6 } ) );
		verify_appended_ranges( appended_to_zip );
	}

	DAW_ATTRIB_NOINLINE void test_zip_more_appends_to_tuple( ) {
		auto const appended_to_tuple = ZipMore( std::array{ 1, 2 } )(
		  std::tuple{ std::array{ 3, 4 }, std::array{ 5, 6 } } );
		verify_appended_ranges( appended_to_tuple );
	}
#endif

	DAW_ATTRIB_NOINLINE void test_concat_materializes_tied_ranges( ) {
		auto first = std::array{ 1, 2, 3 };
		auto second = std::array{ 4, 5 };
		auto concatenated = Concat( std::tie( first, second ) );
		auto result = pipeline( concatenated, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3, 4, 5 } );
	}

	DAW_ATTRIB_NOINLINE void test_concat_preserves_references( ) {
		auto first = std::array{ 1, 2, 3 };
		auto second = std::array{ 4, 5 };
		auto concatenated = Concat( std::tie( first, second ) );
		*concatenated.begin( ) = 10;
		daw_ensure( first.front( ) == 10 );
	}

#if DAW_CPP_VERSION > 202002L
	DAW_ATTRIB_NOINLINE void test_concat_flattens_zip( ) {
		auto const zipped = Zip( std::array{ 1, 2 }, std::array{ 3, 4 } );
		auto zipped_result = pipeline( zipped, Concat, To<std::vector> );
		daw_ensure( zipped_result == std::vector{ 1, 2, 3, 4 } );
	}
#endif

	DAW_ATTRIB_NOINLINE void test_concat_skips_leading_empty_range( ) {
		auto empty = std::array<int, 0>{ };
		auto values = std::array{ 1, 2 };
		auto leading_empty = Concat( std::tie( empty, values ) );
		daw_ensure( *leading_empty.begin( ) == 1 );
	}

	DAW_ATTRIB_NOINLINE void test_concat_iterator_copy_preserves_position( ) {
		auto empty = std::array<int, 0>{ };
		auto values = std::array{ 1, 2 };
		auto concatenated = Concat( std::tie( empty, values ) );
		auto copied_position = concatenated.begin( );
		++copied_position;
		auto copied_iterator = copied_position;
		daw_ensure( *copied_iterator == 2 );
	}

	DAW_ATTRIB_NOINLINE void test_concat_iterator_post_increment( ) {
		auto empty = std::array<int, 0>{ };
		auto values = std::array{ 1, 2 };
		auto concatenated = Concat( std::tie( empty, values ) );
		auto post_incremented = concatenated.begin( );
		auto previous = post_incremented++;
		daw_ensure( *previous == 1 );
		daw_ensure( *post_incremented == 2 );
	}

	DAW_ATTRIB_NOINLINE void test_concat_skips_middle_empty_range( ) {
		auto first = std::array{ 1 };
		auto empty = std::array<int, 0>{ };
		auto last = std::array{ 2 };
		auto concatenated = Concat( std::tie( first, empty, last ) );
		auto flattened = pipeline( concatenated, To<std::vector> );
		daw_ensure( flattened == std::vector{ 1, 2 } );
	}

	DAW_ATTRIB_NOINLINE void test_concat_owns_rvalue_tuple_ranges( ) {
		auto owned =
		  Concat( std::tuple{ std::vector{ 1, 2 }, std::vector{ 3, 4 } } );
		auto owned_values = pipeline( owned, To<std::vector> );
		daw_ensure( owned_values == std::vector{ 1, 2, 3, 4 } );
	}

	DAW_ATTRIB_NOINLINE void test_concat_heterogeneous_iterators_compare( ) {
		auto first = std::array{ 1 };
		auto forward_values = std::forward_list{ 2, 3 };
		auto heterogeneous = Concat( std::tie( first, forward_values ) );
		auto lhs = heterogeneous.begin( );
		auto rhs = heterogeneous.begin( );
		daw_ensure( lhs == rhs );
	}

	DAW_ATTRIB_NOINLINE void test_concat_heterogeneous_end_iterators_compare( ) {
		auto first = std::array{ 1 };
		auto forward_values = std::forward_list{ 2, 3 };
		auto heterogeneous = Concat( std::tie( first, forward_values ) );
		auto lhs = heterogeneous.begin( );
		auto rhs = heterogeneous.begin( );
		while( lhs != heterogeneous.end( ) ) {
			++lhs;
		}
		while( rhs != heterogeneous.end( ) ) {
			++rhs;
		}
		daw_ensure( lhs == rhs );
	}

	DAW_ATTRIB_NOINLINE void test_concat_view_equality( ) {
		auto first_a = std::array{ 1, 2 };
		auto second_a = std::array{ 3, 4 };
		auto concatenated_a = Concat( std::tie( first_a, second_a ) );

		auto first_b = std::array{ 1, 2 };
		auto second_b = std::array{ 3, 4 };
		auto concatenated_b = Concat( std::tie( first_b, second_b ) );

		daw_ensure( concatenated_a == concatenated_b );
		second_b[0] = 99;
		daw_ensure( concatenated_a != concatenated_b );
	}

	DAW_ATTRIB_NOINLINE void test_concat_view_equality_heterogeneous_ranges( ) {
		auto array_a = std::array{ 1 };
		auto list_a = std::forward_list{ 2, 3 };
		auto concatenated_a = Concat( std::tie( array_a, list_a ) );

		auto array_b = std::array{ 1 };
		auto list_b = std::forward_list{ 2, 3 };
		auto concatenated_b = Concat( std::tie( array_b, list_b ) );

		daw_ensure( concatenated_a == concatenated_b );
		list_b.front( ) = 42;
		daw_ensure( concatenated_a != concatenated_b );
	}

	DAW_ATTRIB_NOINLINE void test_concat_duplicate_range_types( ) {
		// first and second are both std::vector<int>&, i.e. the same
		// alternative type appears twice in the underlying variant.
		auto first = std::vector{ 1, 2 };
		auto second = std::vector{ 3, 4, 5 };
		auto concatenated = Concat( std::tie( first, second ) );
		auto result = pipeline( concatenated, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3, 4, 5 } );
	}

	DAW_ATTRIB_NOINLINE void
	test_concat_const_view_dereferences_const_reference( ) {
		const std::array first{ 1, 2 };
		const std::array second{ 3, 4 };
		const auto concatenated = Concat( std::tie( first, second ) );
		daw_ensure( *concatenated.begin( ) == 1 );
		auto result = pipeline( concatenated, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3, 4 } );
	}

	DAW_ATTRIB_NOINLINE void test_concat_all_ranges_empty( ) {
		auto first = std::array<int, 0>{ };
		auto second = std::array<int, 0>{ };
		auto concatenated = Concat( std::tie( first, second ) );
		daw_ensure( concatenated.begin( ) == concatenated.end( ) );
	}

	DAW_ATTRIB_NOINLINE void test_concat_skips_trailing_empty_range( ) {
		auto values = std::array{ 1, 2 };
		auto empty = std::array<int, 0>{ };
		auto concatenated = Concat( std::tie( values, empty ) );
		auto result = pipeline( concatenated, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2 } );

		auto it = concatenated.begin( );
		++it;
		++it;
		daw_ensure( it == concatenated.end( ) );
	}

	DAW_ATTRIB_NOINLINE void test_concat_single_range( ) {
		// std::vector has no tuple_size/tuple_element specialization, so
		// (unlike std::array) it isn't tuple-like and is passed through
		// as a single Range rather than being treated as a tuple of Ranges.
		auto only = std::vector{ 1, 2, 3 };
		auto concatenated = Concat( only );
		auto result = pipeline( concatenated, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3 } );
	}

	DAW_ATTRIB_NOINLINE void test_concat_single_range_array( ) {
		// std::array IS tuple-like, but its elements (int) aren't Ranges,
		// so Concat_t::operator()'s is_tuple_of_fwd_ranges check keeps it
		// out of the tuple-of-ranges branch and passes it through as a
		// single Range instead.
		auto only = std::array{ 1, 2, 3 };
		auto concatenated = Concat( only );
		auto result = pipeline( concatenated, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3 } );
	}

	DAW_ATTRIB_NOINLINE void test_concat_direct_multiple_arguments( ) {
		auto first = std::vector{ 1, 2 };
		auto second = std::vector{ 3 };
		auto third = std::vector{ 4, 5 };
		auto concatenated = Concat( first, second, third );
		auto result = pipeline( concatenated, To<std::vector> );
		daw_ensure( result == std::vector{ 1, 2, 3, 4, 5 } );
	}

	DAW_ATTRIB_NOINLINE void
	test_concat_direct_call_forwards_all_ranges_by_reference( ) {
		// Concat_t::operator()( R &&r, Rs &&...rs ) forwards every
		// argument, so mutating through the concatenated view is visible
		// on both `first` and `second`, not just the first argument.
		auto first = std::vector{ 1, 2 };
		auto second = std::vector{ 3, 4 };
		auto concatenated = Concat( first, second );

		*concatenated.begin( ) = 10;
		daw_ensure( first.front( ) == 10 );

		auto tail = concatenated.begin( );
		++tail;
		++tail;
		*tail = 99;
		daw_ensure( second.front( ) == 99 );
	}

	DAW_ATTRIB_NOINLINE void
	test_concat_heterogeneous_iterator_copy_preserves_position( ) {
		auto first = std::array{ 1 };
		auto forward_values = std::forward_list{ 2, 3 };
		auto heterogeneous = Concat( std::tie( first, forward_values ) );
		auto position = heterogeneous.begin( );
		++position;
		auto copied = position;
		++copied;
		daw_ensure( *position == 2 );
		daw_ensure( *copied == 3 );
	}
} // namespace tests

int main( ) {
#if DAW_CPP_VERSION > 202002L
	tests::test_zip_view_to_map_matches_source_pairs( );
	tests::test_enumerate_and_enumerate_with_map_to_letters( );
#endif
	tests::test_string_pipeline_filter_sort_unique_sum( );
	tests::test_pipeline_composed_as_stage_of_another_pipeline( );
	tests::test_pipeline_callable_reused_across_inputs( );
	tests::test_pipeline_with_source_baked_in( );
	tests::test_consteval_filter_map_optional_count_to_iota( );
	tests::test_swizzle_tuple_elements( );
#if DAW_CPP_VERSION > 202002L
	tests::test_zip_more_appends_range_to_view_and_zip( );
	tests::test_fma_pipeline_matches_manual_computation( );
#endif
	tests::test_skip_then_take( );
	tests::test_iota_to_fixed_array_and_sum( );
#if DAW_CPP_VERSION > 202002L
	tests::test_pipeline_mapapply_clamp_concat_and_conversions( );
#endif
	tests::test_kahan_vs_naive_summation( );
	tests::test_generate_n_to_fixed_array( );
	tests::test_skip_elements( );
	tests::test_sample_from_iota_view( );
	tests::test_reverse_iota_view( );
	tests::test_every_nth_element( );
	tests::test_take_until_predicate( );
	tests::test_take_while_predicate( );
	tests::test_flatten_nested_iota_views( );
	tests::test_chunk_then_flatten_round_trip( );
	tests::test_find_if_and_find_on_array( );
	tests::test_find_with_member_projection( );
	tests::test_slide_then_flatten( );
	tests::test_slide_window_starts( );
	tests::test_contains_string_view_array( );
	tests::test_dump_smoke_test( );
#if DAW_CPP_VERSION > 202002L
	tests::test_enumerate_from_offset( );
#endif
	tests::test_split_by_delimiter_and_trim( );
	tests::test_split_by_subsequence_delimiter( );
#if DAW_CPP_VERSION > 202002L
	tests::test_curried_zip_stage_in_pipeline( );
#endif
	tests::test_trim_both_ends_with_drop_while_reverse( );
	tests::test_trimmed_first_ref_returns_first_char( );
	tests::test_first_ref_reflects_mutation( );
	tests::test_trimmed_first_returns_first_char( );
	tests::test_first_value_does_not_reflect_mutation( );
	tests::test_copy_sort_unique_pipeline( );
#if DAW_CPP_VERSION > 202002L
	tests::test_elements_projection_returns_tuples( );
	tests::test_element_projection_returns_values( );
#endif
	tests::test_cache_last_avoids_recomputation( );
	tests::test_map_function_pointer_projection( );
	tests::test_take_larger_than_forward_range( );
	tests::test_take_from_empty_forward_range( );
	tests::test_take_zero_from_forward_range( );
	tests::test_map_rvalue_projection_dereferences_as_value( );
	tests::test_map_materializes_projected_prvalues( );
	tests::test_const_map_uses_const_iterator( );
	tests::test_const_map_can_be_materialized( );
	tests::test_map_after_const_unique_can_be_summed( );
#if DAW_CPP_VERSION > 202002L
	tests::test_zip_more_appends_to_zip( );
	tests::test_zip_more_appends_to_tuple( );
#endif
	tests::test_concat_materializes_tied_ranges( );
	tests::test_concat_preserves_references( );
#if DAW_CPP_VERSION > 202002L
	tests::test_concat_flattens_zip( );
#endif
	tests::test_concat_skips_leading_empty_range( );
	tests::test_concat_iterator_copy_preserves_position( );
	tests::test_concat_iterator_post_increment( );
	tests::test_concat_skips_middle_empty_range( );
	tests::test_concat_owns_rvalue_tuple_ranges( );
	tests::test_concat_heterogeneous_iterators_compare( );
	tests::test_concat_heterogeneous_end_iterators_compare( );
	tests::test_concat_view_equality( );
	tests::test_concat_view_equality_heterogeneous_ranges( );
	tests::test_concat_duplicate_range_types( );
	tests::test_concat_const_view_dereferences_const_reference( );
	tests::test_concat_all_ranges_empty( );
	tests::test_concat_skips_trailing_empty_range( );
	tests::test_concat_single_range( );
	tests::test_concat_single_range_array( );
	tests::test_concat_direct_multiple_arguments( );
	tests::test_concat_direct_call_forwards_all_ranges_by_reference( );
	tests::test_concat_heterogeneous_iterator_copy_preserves_position( );
	daw::println( "Done" );
}
