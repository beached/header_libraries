// The MIT License (MIT)
//
// Copyright (c) 2018-2020 Darrell Wright
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
#include "daw/daw_bounded_hash_set.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_string_view.h"

constexpr bool test_002( ) {
	size_t count = 1024ULL;
	daw::bounded_hash_set_t<size_t, 1024ULL, daw::fnv1a_hash_t> adapt{};
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
	return true;
}
static_assert( test_002( ) );

constexpr bool test_004( ) {
	size_t count = 1024ULL;
	daw::bounded_hash_set_t<size_t, 1024ULL, daw::fnv1a_hash_t> adapt{};
	for( size_t n = 0; n < count; ++n ) {
		adapt.insert( n );
	}
	for( size_t n = 0; n < count; ++n ) {
		if( !adapt.exists( n ) ) {
			return false;
		}
	}
	return true;
}
static_assert( test_004( ) );

constexpr bool test_005( ) {
	size_t count = 1024ULL;
	daw::bounded_hash_set_t<size_t, 1024ULL, daw::fnv1a_hash_t> adapt{};
	for( size_t n = 0; n < count; n += 2 ) {
		adapt.insert( n );
	}
	for( size_t n = 0; n < count; ++n ) {
		if( !adapt.exists( n ) and n % 2 == 0 ) {
			return false;
		}
	}
	return true;
}
static_assert( test_005( ) );

constexpr bool make_hash_set_001( ) {
	using namespace daw::string_view_literals;
	auto hs =
	  daw::make_bounded_hash_set<daw::string_view>( {"hello"_sv, "there"_sv} );
	return hs.count( "hello" ) != 0;
}
static_assert( make_hash_set_001( ) );

int main( ) {}
