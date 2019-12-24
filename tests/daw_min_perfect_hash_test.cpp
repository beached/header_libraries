// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include "daw/cpp_17.h"
#include "daw/daw_array.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_metro_hash.h"
#include "daw/daw_min_perfect_hash.h"
#include "daw/daw_view.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

struct IntHasher {
	template<typename Integer>
	constexpr size_t operator( )( Integer const &i ) const {
		return static_cast<size_t>( i );
	}
};

struct HashMe {
	template<typename Value>
	constexpr size_t operator( )( Value const &v, size_t seed ) const {
		/*if constexpr( std::is_integral_v<Value> ) {
		  return IntHasher{}( v, seed );
		} else {*/
		return daw::fnv1a_hash_t::append_hash( seed, v );
		//	}
	}

	template<typename Value>
	constexpr size_t operator( )( Value const &v ) const {
		if constexpr( std::is_integral_v<Value> ) {
			return IntHasher{}( v );
		} else {
			return daw::fnv1a_hash_t{}( v );
		}
	}
};

struct MetroHash {
	template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>,
	                                            std::nullptr_t> = nullptr>
	constexpr size_t operator( )( Integer value, size_t seed = 0 ) const {
		char buff[sizeof( Integer )]{};
		for( size_t n = 0; n < sizeof( Integer ); ++n ) {
			buff[n] = static_cast<char>(
			  ( static_cast<uintmax_t>( value ) >> ( n * 8ULL ) ) & 0xFFULL );
		}
		char const *first = buff;
		auto const sz = static_cast<ptrdiff_t>( sizeof( Integer ) );
		char const *last = first + sz;
		return daw::metro::hash64( daw::view<char const *>( first, last ), seed );
	}

	constexpr size_t operator( )( std::string_view sv, size_t seed = 0 ) const {
		return daw::metro::hash64( {sv.begin( ), sv.end( )}, seed );
	}
};
template<size_t N>
using matching_unsigned_t = std::conditional_t<
  N == 64, uint64_t,
  std::conditional_t<
    N == 32, uint32_t,
    std::conditional_t<N == 16, uint16_t,
                       std::conditional_t<N == 8, uint8_t, uintmax_t>>>>;

template<typename Hm, typename Arry>
bool validate( Hm &&hm, Arry const &ary ) {
	for( auto const &item : ary ) {
		auto result = hm[item.first];
		daw::expecting( result == item.second );
	}
	return true;
}

using namespace std::string_view_literals;
constexpr bool test_001( ) {
	constexpr auto ph =
	  daw::make_perfect_hash_table<IntHasher, int, char const *>(
	    {{100, "Continue"},
	     {101, "Switching Protocols"},
	     {102, "Processing"},
	     {200, "OK"},
	     {201, "Created"},
	     {202, "Accepted"},
	     {203, "Non-Authoritative Information"},
	     {204, "No Content"},
	     {205, "Reset Content"},
	     {206, "Partial Content"},
	     {207, "Multi-Status"},
	     {208, "Already Reported"},
	     {226, "IM Used"}} );

	daw::expecting( "Continue"sv, ph[100] );
	daw::expecting( "Switching Protocols"sv, ph[101] );
	daw::expecting( "Processing"sv, ph[102] );
	daw::expecting( "OK"sv, ph[200] );
	daw::expecting( "Created"sv, ph[201] );
	daw::expecting( "Accepted"sv, ph[202] );
	daw::expecting( "Non-Authoritative Information"sv, ph[203] );
	daw::expecting( "No Content"sv, ph[204] );
	daw::expecting( "Reset Content"sv, ph[205] );
	daw::expecting( "Partial Content"sv, ph[206] );
	daw::expecting( "Multi-Status"sv, ph[207] );
	daw::expecting( "Already Reported"sv, ph[208] );
	daw::expecting( "IM Used"sv, ph[226] );
	daw::expecting( nullptr, ph.find( 1234 ) );
	daw::expecting( not ph.contains( 1234 ) );
	daw::expecting( ph.contains( 204 ) );
	return true;
}

constexpr uint32_t u32( char const ( &str )[5] ) {
	uint32_t result = static_cast<uint32_t>( str[0] );
	for( size_t n = 1; n < 4; ++n ) {
		result <<= 8U;
		result |= static_cast<uint32_t>( str[n] );
	}
	return result;
}

inline constexpr std::pair<uint32_t, bool> values[16]{
  {u32( "INFO" ), true}, {u32( "CONN" ), true}, {u32( "PUB " ), true},
  {u32( "SUB " ), true}, {u32( "UNSU" ), true}, {u32( "PING" ), true},
  {u32( "PONG" ), true}, {u32( "+OK " ), true}, {u32( "-ERR" ), true},
  {u32( "AUTH" ), true}, {u32( "PUSH" ), true}, {u32( "ADD " ), true},
  {u32( "DECR" ), true}, {u32( "SET " ), true}, {u32( "GET " ), true},
  {u32( "QUIT" ), true}};

inline constexpr std::pair<std::string_view, bool> values2[16]{
  {"INFO"sv, true}, {"CONN"sv, true}, {"PUB "sv, true}, {"SUB "sv, true},
  {"UNSU"sv, true}, {"PING"sv, true}, {"PONG"sv, true}, {"+OK "sv, true},
  {"-ERR"sv, true}, {"AUTH"sv, true}, {"PUSH"sv, true}, {"ADD "sv, true},
  {"DECR"sv, true}, {"SET "sv, true}, {"GET "sv, true}, {"QUIT"sv, true}};

template<size_t Runs>
void test_min_perf_hash( ) {
	auto const phm_values =
	  daw::make_perfect_hash_table<IntHasher, uint32_t, bool>( values );
	daw::bench_n_test<Runs>(
	  "Minimal Perfect HashMap - uint32_t key",
	  [&]( auto m ) {
		  daw::do_not_optimize( m );
		  size_t result = 0;
		  daw::do_not_optimize( result );
		  for( auto &k : values ) {
			  result += static_cast<size_t>( m[k.first] );
		  }
		  daw::do_not_optimize( result );
	  },
	  phm_values );

	for( auto &k : values ) {
		auto const r = phm_values[k.first];
		daw::expecting( r == k.second );
	}
}

template<size_t Runs>
void test_min_perf_hash2( ) {
	auto phm_values2 =
	  daw::perfect_hash_table<16, std::string_view, bool, MetroHash>( values2 );
	daw::bench_n_test<Runs>(
	  "Minimal Perfect HashMap - string_view key",
	  [&]( auto m ) {
		  daw::do_not_optimize( m );
		  size_t result = 0;
		  daw::do_not_optimize( result );
		  for( auto &k : values2 ) {
			  result += static_cast<size_t>( m[k.first] );
			  daw::do_not_optimize( result );
		  }
		  daw::do_not_optimize( result );
	  },
	  phm_values2 );
}

template<size_t Runs>
void test_unorderd_map( ) {
	auto const hm_values = std::unordered_map<uint32_t, bool>(
	  std::begin( values ), std::end( values ) );
	daw::bench_n_test<Runs>(
	  "std::unordered_map - uint32_t key",
	  [&]( auto m ) {
		  daw::do_not_optimize( m );
		  size_t result = 0;
		  daw::do_not_optimize( result );
		  for( auto &k : values ) {
			  result += static_cast<size_t>( m[k.first] );
			  daw::do_not_optimize( result );
		  }
		  daw::do_not_optimize( result );
	  },
	  hm_values );
}

template<size_t Runs>
void test_unorderd_map2( ) {
	auto const hm_values2 = std::unordered_map<std::string_view, bool>(
	  std::begin( values2 ), std::end( values2 ) );
	daw::bench_n_test<Runs>(
	  "std::unordered_map - string_view key - std::hash",
	  [&]( auto m ) {
		  daw::do_not_optimize( m );
		  size_t result = 0;
		  daw::do_not_optimize( result );
		  for( auto &k : values2 ) {
			  result += static_cast<size_t>( m[k.first] );
			  daw::do_not_optimize( result );
		  }
		  daw::do_not_optimize( result );
	  },
	  hm_values2 );
}

template<size_t Runs>
void test_unorderd_map3( ) {
	auto const hm_values2 = std::unordered_map<std::string_view, bool, MetroHash>(
	  std::begin( values2 ), std::end( values2 ) );
	daw::bench_n_test<Runs>(
	  "std::unordered_map - string_view key - MetroHash",
	  [&]( auto m ) {
		  daw::do_not_optimize( m );
		  size_t result = 0;
		  daw::do_not_optimize( result );
		  for( auto &k : values2 ) {
			  result += static_cast<size_t>( m[k.first] );
			  daw::do_not_optimize( result );
		  }
		  daw::do_not_optimize( result );
	  },
	  hm_values2 );
}

template<typename T0, typename T1, size_t N, size_t... Is>
constexpr auto swap_pairs_impl( std::array<std::pair<T0, T1>, N> &&arry,
                                std::index_sequence<Is...> ) {
	return std::array<std::pair<T1, T0>, N>{std::pair<T1, T0>{
	  std::move( arry[Is] ).second, std::move( arry[Is] ).first}...};
}

template<typename T0, typename T1, size_t N>
constexpr auto swap_pairs( std::array<std::pair<T0, T1>, N> &&arry ) {
	return swap_pairs_impl( std::move( arry ), std::make_index_sequence<N>{} );
}

inline constexpr auto http_response_codes =
  swap_pairs( daw::make_array<std::pair<uint16_t, std::string_view>>(
    {{100, "Continue"},
     {101, "Switching Protocols"},
     {102, "Processing"},
     {200, "OK"},
     {201, "Created"},
     {202, "Accepted"},
     {203, "Non-Authoritative Information"},
     {204, "No Content"},
     {205, "Reset Content"},
     {206, "Partial Content"},
     {207, "Multi-Status"},
     {208, "Already Reported"},
     {226, "IM Used"},
     {300, "Multiple Choices"},
     {301, "Moved Permanently"},
     {302, "Found"},
     {303, "See Other"},
     {304, "Not Modified"},
     {305, "Use Proxy"},
     {307, "Temporary Redirect"},
     {308, "Permanent Redirect"},
     {400, "Bad Request"},
     {401, "Unauthorized"},
     {402, "Payment Required"},
     {403, "Forbidden"},
     {404, "Not Found"},
     {405, "Method Not Allowed"},
     {406, "Not Acceptable"},
     {407, "Proxy Authentication Required"},
     {408, "Request Timeout"},
     {409, "Conflict"},
     {410, "Gone"},
     {411, "Length Required"},
     {412, "Precondition Failed"},
     {413, "Payload Too Large"},
     {414, "URI Too Long"},
     {415, "Unsupported Media Type"},
     {416, "Range Not Satisfiable"},
     {417, "Expectation Failed"},
     {422, "Unprocessable Entity"},
     {423, "Locked"},
     {424, "Failed Dependency"},
     {426, "Upgrade Required"},
     {428, "Precondition Required"},
     {429, "Too Many Requests"},
     {431, "Request Header Fields Too Large"},
     {500, "Internal Server Error"},
     {501, "Not Implemented"},
     {502, "Bad Gateway"},
     {503, "Service Unavailable"},
     {504, "Gateway Timeout"},
     {505, "HTTP Version Not Supported"},
     {506, "Variant Also Negotiates"},
     {507, "Insufficient Storage"},
     {508, "Loop Detected"},
     {510, "Not Extended"},
     {511, "Network Authentication Required"}} ) );

template<size_t Runs>
void test_min_perf_hash3( ) {
	auto phm_resp =
	  daw::perfect_hash_table<std::tuple_size_v<decltype( http_response_codes )>,
	                          std::string_view, uint16_t,
	                          std::hash<std::string_view>>(
	    http_response_codes.begin( ), http_response_codes.end( ) );
	validate( phm_resp, http_response_codes );
	daw::bench_n_test<Runs>(
	  "Minimal Perfect HashMap - string_view key - larger http resp code - "
	  "MetroHash",
	  [&]( auto m ) {
		  daw::do_not_optimize( m );
		  size_t result = 0;
		  daw::do_not_optimize( result );
		  for( auto &k : http_response_codes ) {
			  result += static_cast<size_t>( m[k.first] == k.second );
			  daw::do_not_optimize( result );
		  }
		  daw::do_not_optimize( result );
	  },
	  phm_resp );
}

template<size_t Runs>
void test_unorderd_map4( ) {
	auto hm_resp = std::unordered_map<std::string_view, uint16_t>(
	  http_response_codes.begin( ), http_response_codes.end( ) );

	daw::bench_n_test<Runs>(
	  "unordered_map - string_view key - larger http resp code - std::hash",
	  [&]( auto m ) {
		  daw::do_not_optimize( m );
		  size_t result = 0;
		  daw::do_not_optimize( result );
		  for( auto &k : http_response_codes ) {
			  result += static_cast<size_t>( m[k.first] == k.second );
			  daw::do_not_optimize( result );
		  }
		  daw::do_not_optimize( result );
	  },
	  hm_resp );
}

extern uint16_t http_test_daw( std::string_view sv ) {
	auto phm_resp =
	  daw::perfect_hash_table<std::tuple_size_v<decltype( http_response_codes )>,
	                          std::string_view, uint16_t, MetroHash>(
	    http_response_codes.begin( ), http_response_codes.end( ) );
	return phm_resp[sv];
}

#if defined( DEBUG )
inline constexpr size_t Runs = 100;
#else
inline constexpr size_t Runs = 100'00;
#endif

int main( ) {
	test_001( );
	test_min_perf_hash<Runs>( );
	test_unorderd_map<Runs>( );
	test_min_perf_hash2<Runs>( );
	test_unorderd_map2<Runs>( );
	test_unorderd_map3<Runs>( );
	test_min_perf_hash3<Runs>( );
	test_unorderd_map4<Runs>( );
	return 0;
}
