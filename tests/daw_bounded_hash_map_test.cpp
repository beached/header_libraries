// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_bounded_hash_map.h"
#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_string_view.h"
#include "daw/daw_utility.h"

#include <cstddef>
#include <cstdint>

namespace {
	constexpr daw::bounded_hash_map<uint16_t, daw::string_view, 13,
	                                daw::fnv1a_hash_t> const status_codes{
	  { { 100, "Continue" },
	    { 101, "Switching Protocols" },
	    { 102, "Processing" },
	    { 200, "OK" },
	    { 201, "Created" },
	    { 202, "Accepted" },
	    { 203, "Non-Authoritative Information" },
	    { 204, "No Content" },
	    { 205, "Reset Content" },
	    { 206, "Partial Content" },
	    { 207, "Multi-Status" },
	    { 208, "Already Reported" },
	    { 226, "IM Used" } } };

	constexpr auto const status_codes2 =
	  daw::make_bounded_hash_map<uint16_t, daw::string_view, daw::fnv1a_hash_t>(
	    { { 100, "Continue" },
	      { 101, "Switching Protocols" },
	      { 102, "Processing" },
	      { 200, "OK" },
	      { 201, "Created" },
	      { 202, "Accepted" },
	      { 203, "Non-Authoritative Information" },
	      { 204, "No Content" },
	      { 205, "Reset Content" },
	      { 206, "Partial Content" },
	      { 207, "Multi-Status" },
	      { 208, "Already Reported" },
	      { 226, "IM Used" } } );
} // namespace

constexpr bool test_const_001( ) noexcept {
	auto const &hm = status_codes;
	::Unused( status_codes2 );
	auto t = *hm.find( 204 );
	::Unused( t );
	return hm.exists( 204 );
}
static_assert( test_const_001( ) );

constexpr int
test_const_002( daw::bounded_hash_map<daw::string_view, int, 100> const &m,
                daw::string_view k ) noexcept {
	return m[k];
}

constexpr int
test_const_002( daw::bounded_hash_map<daw::string_view, int, 100> &m,
                daw::string_view k ) noexcept {
	return m[k];
}

int main( ) {}
