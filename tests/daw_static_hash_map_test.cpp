// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_static_hash_map.h"
#include "daw/daw_string_view.h"

namespace {
	constexpr daw::static_hash_map<uint16_t, daw::string_view, 13, daw::fnv1a_hash_t> const
	  status_codes{{{100, "Continue"},
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
	                {226, "IM Used"}}};

	constexpr auto const status_codes2 =
	  daw::make_static_hash_map<uint16_t, daw::string_view, daw::fnv1a_hash_t>(
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
} // namespace


constexpr bool test_const_001( ) noexcept {
	auto const & hm = status_codes;
	auto t = *hm.find( 204 );
	::Unused( t );
	return hm.exists( 204 );
}
static_assert( test_const_001( ) );

constexpr int test_const_002( daw::static_hash_map<daw::string_view, int, 100> const & m, daw::string_view k ) noexcept {
		return m[k];
}

constexpr int test_const_002( daw::static_hash_map<daw::string_view, int, 100> & m, daw::string_view k ) noexcept {
		return m[k];
}

int main( ) {
}

