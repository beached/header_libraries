// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_ref_storage.h>

#include <compare>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

static_assert(
  std::same_as<decltype( daw::ref_storage{ 42 } ), daw::ref_storage<int>> );
static_assert(
  std::same_as<decltype( daw::ref_storage{ std::declval<int &>( ) } ),
               daw::ref_storage<int &>> );
static_assert(
  std::same_as<decltype( daw::ref_storage{ std::declval<int const &>( ) } ),
               daw::ref_storage<int const &>> );

static_assert(
  std::same_as<decltype( std::declval<daw::ref_storage<int> &>( ).get( ) ),
               int &> );
static_assert(
  std::same_as<
    decltype( std::declval<daw::ref_storage<int> const &>( ).get( ) ),
    int const &> );
static_assert(
  std::same_as<
    decltype( std::declval<daw::ref_storage<int &> const &>( ).get( ) ),
    int const &> );
static_assert( std::convertible_to<daw::ref_storage<int> &, int &> );
static_assert(
  std::convertible_to<daw::ref_storage<int> const &, int const &> );
static_assert( sizeof( daw::ref_storage<int &> ) == sizeof( int * ) );

static_assert( std::default_initializable<daw::ref_storage<int>> );
static_assert( not std::default_initializable<daw::ref_storage<int &>> );
static_assert( daw::constructible<daw::ref_storage<std::string>,
                                       std::in_place_t, std::size_t, char> );

constexpr bool test_value_storage( ) {
	auto value = daw::ref_storage{ 42 };
	value.get( ) = 7;

	auto const &const_value = value;
	auto same = daw::ref_storage{ 7 };
	auto larger = daw::ref_storage{ 8 };
	return value == same and value != larger and value == 7 and 7 == value and
	       const_value.get( ) == 7 and
	       ( value <=> 8 ) == std::strong_ordering::less;
}

constexpr bool test_reference_storage( ) {
	int source = 42;
	auto value = daw::ref_storage{ source };
	if( &value.get( ) != &source ) {
		return false;
	}

	value.get( ) = 7;
	auto copy = value;
	copy.get( ) = 9;
	auto in_place = daw::ref_storage<int &>{ std::in_place, source };
	return source == 9 and &copy.get( ) == &source and
	       &in_place.get( ) == &source;
}

constexpr bool test_const_reference_storage( ) {
	int const source = 42;
	auto value = daw::ref_storage{ source };
	return &value.get( ) == &source and value == source;
}

static_assert( test_value_storage( ) );
static_assert( test_reference_storage( ) );
static_assert( test_const_reference_storage( ) );

int main( ) {
	auto text =
	  daw::ref_storage<std::string>{ std::in_place, std::size_t{ 4 }, 'a' };
	daw_ensure( text.get( ) == "aaaa" );

	auto owned = daw::ref_storage{ std::make_unique<int>( 42 ) };
	daw_ensure( *owned.get( ) == 42 );

	auto pointer = std::make_unique<int>( 7 );
	auto referenced = daw::ref_storage{ pointer };
	daw_ensure( &referenced.get( ) == &pointer );
	*referenced.get( ) = 9;
	daw_ensure( *pointer == 9 );
}
