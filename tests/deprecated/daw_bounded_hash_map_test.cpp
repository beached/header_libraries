// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/deprecated/daw_bounded_hash_map.h"

#include "daw/daw_fnv1a_hash.h"
#include "daw/daw_string_view.h"
#include "daw/daw_utility.h"

#include <cstddef>
#include <cstdint>

namespace {
	using namespace daw::string_view_literals;
	constexpr daw::
	  bounded_hash_map<uint16_t, daw::string_view, 13, daw::fnv1a_hash_t>
	    status_codes{ { { 100, "Continue"_sv },
	                    { 101, "Switching Protocols"_sv },
	                    { 102, "Processing"_sv },
	                    { 200, "OK"_sv },
	                    { 201, "Created"_sv },
	                    { 202, "Accepted"_sv },
	                    { 203, "Non-Authoritative Information"_sv },
	                    { 204, "No Content"_sv },
	                    { 205, "Reset Content"_sv },
	                    { 206, "Partial Content"_sv },
	                    { 207, "Multi-Status"_sv },
	                    { 208, "Already Reported"_sv },
	                    { 226, "IM Used"_sv } } };

	constexpr auto const status_codes2 =
	  daw::make_bounded_hash_map<uint16_t, daw::string_view, daw::fnv1a_hash_t>(
	    { { 100, "Continue"_sv },
	      { 101, "Switching Protocols"_sv },
	      { 102, "Processing"_sv },
	      { 200, "OK"_sv },
	      { 201, "Created"_sv },
	      { 202, "Accepted"_sv },
	      { 203, "Non-Authoritative Information"_sv },
	      { 204, "No Content"_sv },
	      { 205, "Reset Content"_sv },
	      { 206, "Partial Content"_sv },
	      { 207, "Multi-Status"_sv },
	      { 208, "Already Reported"_sv },
	      { 226, "IM Used"_sv } } );
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
