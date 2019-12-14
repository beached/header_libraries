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

#include "daw/daw_benchmark.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_min_perfect_hash.h"

struct IntHasher {
	template<typename Integer>
	constexpr size_t operator( )( Integer const &i, size_t seed ) const {
		return static_cast<size_t>( i ) ^ seed;
	}
	template<typename Integer>
	constexpr size_t operator( )( Integer const &i ) const {
		return static_cast<size_t>( i ) ^ daw::impl::fnv_prime( );
	}
};

struct HashMe {
	template<typename Value>
	constexpr size_t operator( )( Value const &v, size_t seed ) const {
		if constexpr( std::is_integral_v<Value> ) {
			return IntHasher{}( v, seed );
		} else {
			return daw::fnv1a_hash_t{}( v, seed );
		}
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

inline constexpr std::pair<int, char const *> data_items[13] = {
  {100, "Continue"},
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
  {226, "IM Used"}};

inline constexpr auto ph =
  daw::perfect_hash_table<13, int, char const *, HashMe>( data_items );

using namespace std::string_view_literals;
static_assert( ph[100] == "Continue"sv );
static_assert( ph[101] == "Switching Protocols"sv );
static_assert( ph[102] == "Processing"sv );
static_assert( ph[200] == "OK"sv );
static_assert( ph[201] == "Created"sv );
static_assert( ph[202] == "Accepted"sv );
static_assert( ph[203] == "Non-Authoritative Information"sv );
static_assert( ph[204] == "No Content"sv );
static_assert( ph[205] == "Reset Content"sv );
static_assert( ph[206] == "Partial Content"sv );
static_assert( ph[207] == "Multi-Status"sv );
static_assert( ph[208] == "Already Reported"sv );
static_assert( ph[226] == "IM Used"sv );
static_assert( ph.find( 1234 ) == ph.end( ) );
static_assert( not ph.contains( 1234 ) );
static_assert( ph.contains( 204 ) );

char const *lookup( int key ) {
	return ph[key];
}

int main( ) {
	daw::expecting( ph.contains( 207 ) );
	return 0;
}
