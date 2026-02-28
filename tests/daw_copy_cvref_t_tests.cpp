// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/traits/daw_copy_cvref_t.h>

#include <type_traits>

using daw::copy_const_t;
using daw::copy_cv_t;
using daw::copy_cvref_t;
using daw::copy_reference_t;
using daw::copy_volatile_t;

static_assert( std::is_same_v<copy_reference_t<int &, long>, long &> );
static_assert( std::is_same_v<copy_reference_t<int &&, long>, long &&> );
static_assert( std::is_same_v<copy_reference_t<int const, long>, long> );
static_assert( std::is_same_v<copy_reference_t<int &, long &&>, long &> );
static_assert( std::is_same_v<copy_reference_t<int &&, long &>, long &&> );
static_assert( std::is_same_v<copy_reference_t<int, long &>, long> );

static_assert( std::is_same_v<copy_const_t<int const, long>, long const> );
static_assert( std::is_same_v<copy_const_t<int, long const>, long const> );
static_assert( std::is_same_v<copy_const_t<int, long>, long> );
static_assert( std::is_same_v<copy_const_t<int const, long &>, long const &> );
static_assert( std::is_same_v<copy_const_t<int const &, long>, long const> );
static_assert(
  std::is_same_v<copy_const_t<int const &&, long &&>, long const &&> );

static_assert(
  std::is_same_v<copy_volatile_t<int volatile, long>, long volatile> );
static_assert(
  std::is_same_v<copy_volatile_t<int, long volatile>, long volatile> );
static_assert( std::is_same_v<copy_volatile_t<int, long>, long> );
static_assert(
  std::is_same_v<copy_volatile_t<int volatile &, long>, long volatile> );
static_assert(
  std::is_same_v<copy_volatile_t<int volatile &&, long &&>, long volatile &&> );

static_assert(
  std::is_same_v<copy_cv_t<int const volatile, long>, long const volatile> );
static_assert(
  std::is_same_v<copy_cv_t<int const, long volatile>, long const volatile> );
static_assert(
  std::is_same_v<copy_cv_t<int volatile, long const>, long const volatile> );
static_assert(
  std::is_same_v<copy_cv_t<int, long const volatile>, long const volatile> );
static_assert( std::is_same_v<copy_cv_t<int const &, long &&>, long const &&> );
static_assert(
  std::is_same_v<copy_cv_t<int volatile &&, long &>, long volatile &> );

static_assert( std::is_same_v<copy_cvref_t<int const &, long>, long const &> );
static_assert(
  std::is_same_v<copy_cvref_t<int volatile &, long>, long volatile &> );
static_assert( std::is_same_v<copy_cvref_t<int const volatile &, long>,
                              long const volatile &> );
static_assert(
  std::is_same_v<copy_cvref_t<int const &&, long>, long const &&> );
static_assert(
  std::is_same_v<copy_cvref_t<int volatile &&, long>, long volatile &&> );
static_assert( std::is_same_v<copy_cvref_t<int const volatile &&, long>,
                              long const volatile &&> );
static_assert(
  std::is_same_v<copy_cvref_t<int const volatile, long>, long const volatile> );
static_assert( std::is_same_v<copy_cvref_t<int &, long>, long &> );
static_assert( std::is_same_v<copy_cvref_t<int &, long &&>, long &> );
static_assert( std::is_same_v<copy_cvref_t<int, long &>, long> );
static_assert(
  std::is_same_v<copy_cvref_t<int, long const volatile &>, long const volatile> );

static_assert(
  std::is_same_v<copy_cvref_t<int const &, long &&>, long const &> );

static_assert( std::is_same_v<copy_cv_t<int &&, long const &>, long const &> );
static_assert(
  std::is_same_v<copy_cvref_t<int &&, long const &>, long const &&> );

int main( ) {}
