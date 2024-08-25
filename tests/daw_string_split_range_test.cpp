// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_benchmark.h"
#include "daw/deprecated/daw_string_split_range.h"
#include "daw/deprecated/daw_string_view1.h"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

constexpr bool splt_test_000( ) {
	daw::sv1::string_view const str = "This is a test of the split";
	std::array<daw::sv1::string_view, 7> const expected_tst = {
	  "This", "is", "a", "test", "of", "the", "split" };

	auto rng = daw::split_string( str, " " );

	bool const ans = daw::algorithm::equal(
	  rng.begin( ), rng.end( ), expected_tst.cbegin( ), expected_tst.cend( ) );

	return ans;
}
static_assert( splt_test_000( ) );

void string_split_range_001( ) {
	std::string const str = "This is a test of the split";
	std::vector<std::string> const expected_tst = {
	  "This", "is", "a", "test", "of", "the", "split" };
	std::vector<std::string> tst{ };
	std::string delem = " ";
	daw::string_split_range<char> rng = daw::split_string( str, delem );

	daw::algorithm::transform(
	  rng.begin( ), rng.end( ), std::back_inserter( tst ), []( auto sv ) {
		  return sv.to_string( );
	  } );

	bool const ans = daw::algorithm::equal(
	  tst.cbegin( ), tst.cend( ), expected_tst.cbegin( ), expected_tst.cend( ) );
	daw::expecting( ans );
}

void string_split_range_002( ) {
	std::vector<std::string> const expected_tst = {
	  "This", "is", "a", "test", "of", "the", "split" };
	std::vector<std::string> tst{ };

	for( auto sv : daw::split_string( "This is a test of the split", " " ) ) {
		tst.push_back( sv.to_string( ) );
	}

	bool const ans = std::equal(
	  tst.cbegin( ), tst.cend( ), expected_tst.cbegin( ), expected_tst.cend( ) );
	daw::expecting( ans );
}

void string_split_range_003( ) {
	std::vector<std::string> const expected_tst = {
	  "This", "is", "a", "test", "of", "the", "split" };
	std::vector<std::string> tst{ };

	for( auto sv :
	     daw::split_string( "This  is  a  test  of  the  split", "  " ) ) {
		tst.push_back( sv.to_string( ) );
	}

	bool const ans = std::equal(
	  tst.cbegin( ), tst.cend( ), expected_tst.cbegin( ), expected_tst.cend( ) );
	daw::expecting( ans );
}

int main( ) {
	string_split_range_001( );
	string_split_range_002( );
	string_split_range_003( );
}
