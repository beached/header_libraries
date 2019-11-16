// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights home/dwright/github/lib_nodepp/ to use, copy, modify, merge, publish,
// distribute, sublicense, and / or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the following
// conditions:
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

#include <iostream>

#include "daw/cpp_17.h"
#include "daw/daw_benchmark.h"

constexpr int plus( int a, int b ) {
	return a + b;
}

static_assert( daw::apply( plus, std::make_tuple( 1, 2 ) ) == 3 );
/*
namespace void_t_tests {
  void test2( int ) {}
  struct X {};

  constexpr bool fn2( ... ) {
    return true;
  }

  template<typename T, typename = ::std::void_t<decltype( fn( T{} ) )>>
  constexpr bool fn2( T ) {
    return false;
  }

  constexpr bool void_t_test_001( ) {
    auto const b1 = fn2( X{} );
    daw::expecting( b1 );
    return true;
  }
  static_assert( void_t_test_001( ) );
} // namespace void_t_tests
*/
/*
namespace cpp_17_test_01 {
  void test( ) {}
  struct test2 {};
  static_assert( std::is_function_v<decltype( test )>, "" );
  static_assert( !std::is_function_v<test2>, "" );
} // namespace cpp_17_test_01
*/

namespace not_fn_test {
	struct not_fn_test {
		bool value = false;
		constexpr bool operator( )( ) const noexcept {
			return value;
		}
	};

	static_assert( daw::not_fn<not_fn_test>( )( ), "" );

	static constexpr auto fn1 = not_fn_test{true};
	static constexpr auto const fn2 = daw::not_fn( fn1 );
	static_assert( fn1( ), "" );
	static_assert( !fn2( ), "" );
} // namespace not_fn_test

/*
namespace is_array_v_001 {
  struct A {};
  static_assert( !std::is_array_v<A>, "" );
  static_assert( std::is_array_v<A[]>, "" );
  static_assert( std::is_array_v<A[3]>, "" );
  static_assert( !std::is_array_v<float>, "" );
} // namespace is_array_v_001
*/
namespace advance_001 {
	constexpr bool test_advance_pos( ) {
		int a[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
		auto first = &a[0];
		daw::advance( first, 4 );
		return *first == 4;
	}
	static_assert( test_advance_pos( ), "" );
} // namespace advance_001

namespace advance_002 {
	constexpr bool test_advance_neg( ) {
		int a[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
		auto first = &a[0];
		daw::advance( first, 4 );
		daw::advance( first, -4 );
		return *first == 0;
	}
	static_assert( test_advance_neg( ), "" );
} // namespace advance_002

namespace advance_003 {
	constexpr bool test_advance_zero( ) {
		int a[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
		auto first = &a[0];
		daw::advance( first, 0 );
		return *first == 0;
	}
	static_assert( test_advance_zero( ), "" );
} // namespace advance_003

namespace trivially_destructable_001 {
	struct Foo {
		~Foo( ) = default;
	};
	static_assert( std::is_trivially_destructible_v<Foo>, "" );
} // namespace trivially_destructable_001

void bit_cast_001( ) {
	constexpr uint32_t const tst = 0x89AB'CDEF;
	auto const f = daw::bit_cast<float>( tst );
	auto const i = daw::bit_cast<uint32_t>( f );
	daw::expecting( tst, i );
}

void bit_cast_002( ) {
	constexpr uint32_t const tst = 0x89AB'CDEF;
	auto const f = daw::bit_cast<float>( &tst );
	auto const i = daw::bit_cast<uint32_t>( f );
	daw::expecting( tst, i );
}

void bit_cast_003( ) {
	constexpr uint64_t const as_bin =
	  0b0'01111111111'0000000000000000000000000000000000000000000000000000; // double
	                                                                        // 1
	constexpr double const as_dbl = 1.0;
	bool const b1 = daw::bit_cast<uint64_t>( as_dbl ) == as_bin;
	daw::expecting( b1 );

	constexpr uint64_t const as_bin2 =
	  0b1'10000000000'0000000000000000000000000000000000000000000000000000; // double
	                                                                        // -2
	constexpr double const as_dbl2 = -2.0;

	bool const b2 = daw::bit_cast<uint64_t>( as_dbl2 ) == as_bin2;
	::Unused( b2 );
	daw::expecting( b1 );
}

namespace is_nothrow_convertible_001 {
	struct tmp_t {};

	struct tmp2_t {
		constexpr tmp2_t( tmp_t ) noexcept {}
		constexpr tmp2_t( int ) noexcept( false ) {}
	};

	static_assert( !daw::is_nothrow_convertible_v<int, tmp_t>,
	               "int should not be convertible to tmp_t" );
	static_assert( daw::is_nothrow_convertible_v<int, double>,
	               "int should not be convertible to tmp_t" );
	static_assert( daw::is_nothrow_convertible_v<tmp_t, tmp2_t>,
	               "tmp_t should be nothrow convertible to tmp2_t" );
	static_assert( !daw::is_nothrow_convertible_v<int, tmp2_t>,
	               "int should not be nothrow convertible to tmp2_t" );
} // namespace is_nothrow_convertible_001

namespace decay_copy_001 {
	template<typename T>
	auto dc_func( T &&v ) {
		auto x = std::pair<T, int>( std::forward<T>( v ), 99 );
		return x;
	}

	static double d = 5.6;
	static double const cd = 3.4;

	static_assert(
	  !std::is_same_v<decltype( dc_func( d ) ), decltype( dc_func( cd ) )>,
	  "Results should be difference, double and double const" );

	static_assert( std::is_same_v<decltype( daw::decay_copy( d ) ),
	                              decltype( daw::decay_copy( cd ) )>,
	               "Results should be the same, double" );
} // namespace decay_copy_001

static_assert( daw::any_true_v<false, false, true> );
static_assert( !daw::any_true_v<false, false, false> );

constexpr int add_numbers( int const &lhs, int const &rhs ) {
	return lhs + rhs;
}

static_assert( []( ) {
	int a = 5;
	auto add_to_5 = ::daw::bind_front( &add_numbers, a );
	daw::expecting( add_to_5( 6 ), 11 );
	return true;
}( ) );

int main( ) {
	bit_cast_001( );
	bit_cast_002( );
	bit_cast_003( );
}
