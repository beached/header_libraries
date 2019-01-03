// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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

#include <cstdint>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "daw/boost_test.h"
#include "daw/daw_string_view.h"
#include "daw/daw_traits.h"
#include "daw/daw_utility.h"

namespace daw_traits_is_equality_comparable {
	static_assert( daw::traits::is_equality_comparable_v<std::string>,
	               "1. std::string should report as being equality comparable" );
	static_assert(
	  daw::traits::is_equality_comparable_v<std::vector<std::string>>,
	  "2. std::vector should report as being equality comparable" );

	struct NotEqual {
		int x;
	};
	static_assert(
	  !daw::traits::is_equality_comparable_v<NotEqual>,
	  "3. NotEqual struct should not report as being equality comparable" );

	class NotEqual2 {
	private:
		int x;
		std::vector<int> y;

	public:
		NotEqual2( int, std::vector<int> )
		  : x( 0 )
		  , y( ) {
			x += 1;
		}
	};

	static_assert(
	  !daw::traits::is_equality_comparable_v<NotEqual2>,
	  "4. NotEqual2 struct should not report as being equality comparable" );

} // namespace daw_traits_is_equality_comparable

namespace daw_traits_is_regular {
	static_assert( daw::traits::is_regular_v<std::string>,
	               "1. std::string should report as being regular" );
	struct NotRegular {
		int x;
		NotRegular( int )
		  : x( ) {}
		NotRegular( ) = delete;
		NotRegular( NotRegular const & ) = delete;
		NotRegular &operator=( NotRegular const & ) = delete;
		NotRegular( NotRegular && ) = delete;
		NotRegular &operator=( NotRegular && ) = delete;
	};

	static_assert( !daw::traits::is_regular_v<NotRegular>,
	               "2. struct NotRegular should report as being regular" );
} // namespace daw_traits_is_regular

namespace daw_traits_max_sizeof {
	static_assert( daw::traits::max_sizeof<int8_t, int16_t, uint8_t, int, int32_t,
	                                       int64_t, uint64_t, size_t>::value ==
	                 sizeof( size_t ),
	               "1. Max sizeof did not report the size of the largest item" );
}

namespace daw_traits_are_true {
	static_assert( daw::traits::are_true( true, true, 1, true ),
	               "1. are_true should have reported true" );
	static_assert( !daw::traits::are_true( true, true, false, true, 0 ),
	               "2. are_true should have reported false" );

	static_assert( !daw::traits::are_true( true, 0 ),
	               "3. are_true should have reported false" );
} // namespace daw_traits_are_true

namespace daw_traits_are_same_types {
	static_assert(
	  !daw::traits::are_same_types_v<bool, std::string, std::vector<int>>,
	  "1. Different types reported as same" );
	static_assert(
	  daw::traits::are_same_types_v<std::string, std::string, std::string>,
	  "2. Same types reported as different" );
} // namespace daw_traits_are_same_types

namespace daw_traits_bool_and {
	static_assert( daw::traits::bool_and<true, true, true>::value,
	               "1. All true's in bool_and should return true" );
	static_assert(
	  !daw::traits::bool_and<true, false, false>::value,
	  "2. A mix of true's and false's in bool_and should return false" );

	static_assert( daw::traits::bool_and<true>::value,
	               "3. A true in bool_and should return true" );
	static_assert( !daw::traits::bool_and<false>::value,
	               "4. A false in bool_and should return false" );
} // namespace daw_traits_bool_and

namespace daw_traits_bool_or {
	static_assert( daw::traits::bool_or<true, true, true>::value,
	               "1. All true's in bool_or should return true" );

	static_assert(
	  daw::traits::bool_or<true, false, false>::value,
	  "2. A mix of true's and false's in bool_or should return true" );

	static_assert( daw::traits::bool_or<true>::value,
	               "3. A true in bool_or should return true" );

	// Seems to fail in MSVC 2013, not clang++ or g++
	static_assert( !daw::traits::bool_or<false>::value,
	               "4. A false in bool_or should return false" );

	static_assert( !daw::traits::bool_or<false, false>::value,
	               "5. Two false's in bool_or should return false" );
} // namespace daw_traits_bool_or

bool enable_if_any_func_test( std::string, ... ) {
	return false;
}

template<typename... Args, typename = typename daw::traits::enable_if_any<
                             bool, std::is_same<bool, Args>::value...>::type>
constexpr bool enable_if_any_func_test( Args... args ) {
	return true;
}

constexpr bool enable_if_all_func_test( bool ) {
	return false;
}

template<typename... Args, typename = typename daw::traits::enable_if_all<
                             bool, std::is_same<bool, Args>::value...>::type>
constexpr bool enable_if_all_func_test( Args... args ) {
	return true;
}

namespace daw_traits_enable_if_all {
	static_assert( !enable_if_all_func_test( true ),
	               "1. Enable if all should have defaulted to string "
	               "only with a std::string value" );

	static_assert(
	  enable_if_all_func_test( true, true, true, true, true ),
	  "2. Enable if all should have ran templated version with multiple "
	  "params including a boolean but failed and defaulted to non-templated "
	  "version" );
} // namespace daw_traits_enable_if_all

namespace daw_traits_is_one_of {
	static_assert( daw::traits::is_one_of_v<std::string, std::string, std::string,
	                                        int, std::vector<std::string>>,
	               "1. Is one of should report true when at least one matches" );

	static_assert( !daw::traits::is_one_of_v<std::string, int, int, double,
	                                         std::vector<std::string>>,
	               "2. Is one of should report false when none matches" );

	static_assert(
	  daw::traits::is_one_of_v<std::string, std::string>,
	  "3. Is one of should report true with a single param and it matches" );

	static_assert( !daw::traits::is_one_of_v<std::string, int>,
	               "4. Is one of should report false with a single "
	               "param and it does not match" );
} // namespace daw_traits_is_one_of

namespace daw_traits_has_begin_member {
	static_assert( daw::traits::has_begin_member_v<std::string>,
	               "1. std::string should have a begin( ) method" );

	static_assert( daw::traits::has_begin_member_v<std::vector<int>>,
	               "2. std::vector should have a begin( ) method" );
	using test_t =
	  std::unordered_map<std::string, int>; // Macro's and comma parameters
	static_assert( daw::traits::has_begin_member_v<test_t>,
	               "3. std::unordered should have a begin( ) method" );
	struct T {
		int x;
	};
	static_assert( !daw::traits::has_begin_member_v<T>,
	               "4. struct T should not have a begin( ) method" );
} // namespace daw_traits_has_begin_member

namespace daw_traits_has_substr_member {
	static_assert( daw::traits::has_substr_member_v<std::string>,
	               "1. std::string should have a substr method" );

	static_assert( !daw::traits::has_substr_member_v<std::vector<int>>,
	               "2. std::vector should not have a substr method" );

	using test_t =
	  std::unordered_map<std::string, int>; // Macro's and comma parameters
	static_assert( !daw::traits::has_substr_member_v<test_t>,
	               "3. std::unordered should not have a substr method" );

	struct T {
		int x;
		constexpr bool substr( size_t, size_t ) const noexcept {
			return true;
		}
	};
	constexpr T t = {5};
	static_assert( t.substr( 5, 5 ), "" );
	static_assert( daw::traits::has_substr_member_v<T>,
	               "4. struct T should have a substr method" );
} // namespace daw_traits_has_substr_member

namespace daw_traits_has_value_type_member {
	static_assert( daw::traits::has_value_type_member_v<std::string>,
	               "1. std::string should have a value_type" );
	struct T {
		int x;
	};
	static_assert( !daw::traits::has_value_type_member_v<T>,
	               "2. T should not have a value_type" );
	static_assert( !daw::traits::has_value_type_member_v<int>,
	               "3. int should not have a value_type" );
} // namespace daw_traits_has_value_type_member

namespace daw_traits_is_container_like {
	static_assert( daw::traits::is_container_like_v<std::string>,
	               "1. std::string should be container like" );
	static_assert( daw::traits::is_container_like_v<std::vector<std::string>>,
	               "2. std::vector<std::string> should be container like" );
	using map_t = std::unordered_map<std::string, int>;
	static_assert(
	  daw::traits::is_container_like_v<map_t>,
	  "3. std::unordered_map<std::string, int> should be container like" );
	struct T {
		int x;
	};
	static_assert( !daw::traits::is_container_like_v<T>,
	               "4. T should not be container like" );
	static_assert( !daw::traits::is_container_like_v<int>,
	               "5. int should not be container like" );
} // namespace daw_traits_is_container_like

namespace daw_traits_is_string {
	static_assert( daw::traits::is_string_v<std::string>,
	               "1. is_string should return true for std::string" );
	static_assert(
	  !daw::traits::is_string_v<std::vector<std::string>>,
	  "2. is_string should return false for std::vector<std::string>" );
	struct T {
		int x;
	};
	static_assert( !daw::traits::is_string_v<std::vector<std::string>>,
	               "3. is_string should return false for struct T" );
	static_assert( daw::traits::is_string_v<decltype( "this is a test" )>,
	               "4. Character array should convert to string" );
	static_assert( !daw::traits::is_string_v<std::vector<char>>,
	               "5. Vector of char should not convert to string" );
} // namespace daw_traits_is_string

namespace daw_traits_isnt_string {
	static_assert( !daw::traits::isnt_string_v<std::string>,
	               "1. isnt_string should return true for std::string" );
	static_assert(
	  daw::traits::isnt_string_v<std::vector<std::string>>,
	  "2. isnt_string should return false for std::vector<std::string>" );
	struct T {
		int x;
	};
	static_assert( daw::traits::isnt_string_v<std::vector<std::string>>,
	               "3. isnt_string should return false for struct T" );
	static_assert( !daw::traits::isnt_string_v<decltype( "this is a test" )>,
	               "4. Character array should convert to string" );
	static_assert( daw::traits::isnt_string_v<std::vector<char>>,
	               "5. Vector of char should not convert to string" );
} // namespace daw_traits_isnt_string

struct TestNoOS {
	int x;
	TestNoOS( )
	  : x( 1 ) {}
};

struct TestYesOS {
	int x;
	TestYesOS( )
	  : x( 1 ) {}
};

std::ostream &operator<<( std::ostream &os, TestYesOS const &t ) {
	os << t.x;
	return os;
}

namespace daw_traits_is_streamable {
	constexpr auto tst1 = daw::traits::is_streamable_v<std::ostream, int>;
	static_assert( tst1, "1. int should have an ostream overload" );

	constexpr auto tst2 = daw::traits::is_streamable_v<std::ostream, TestNoOS>;
	static_assert( !tst2, "2. TestNoOS should not have an ostream overload" );

	constexpr auto tst3 = daw::traits::is_streamable_v<std::ostream, TestYesOS>;
	static_assert( tst3, "3. TestYesOS should have an ostream overload" );
} // namespace daw_traits_is_streamable

namespace daw_traits_is_mixed_from_ns {
	template<typename Derived>
	struct Base {
		Base( ) = default;
	};
	struct Derived : public Base<Derived> {
		Derived( ) = default;
	};
	struct NonDerived {
		NonDerived( ) = default;
	};
} // namespace daw_traits_is_mixed_from_ns

namespace daw_traits_is_mixed_from {
	static_assert(
	  daw::traits::is_mixed_from_v<daw_traits_is_mixed_from_ns::Base,
	                               daw_traits_is_mixed_from_ns::Derived>,
	  "1. Base<Child> should be a base for Child" );

	static_assert(
	  !daw::traits::is_mixed_from_v<daw_traits_is_mixed_from_ns::Base,
	                                daw_traits_is_mixed_from_ns::NonDerived>,
	  "2. Base<NonDerived> should not be a base for NonDerived" );
} // namespace daw_traits_is_mixed_from

constexpr int f( int ) noexcept {
	return 1;
}

namespace daw_traits_is_callable {
#ifdef __clang__
#ifndef __ICC // icpc defines the __clang__ macro
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif
#endif
	auto const blah = []( auto t ) noexcept {
		return t;
	};
#ifdef __clang__
#ifndef __ICC // icpc defines the __clang__ macro
#pragma clang diagnostic pop
#endif
#endif

	static_assert( daw::traits::is_callable_v<decltype( blah ), int>,
	               "blah should be callable" );

	constexpr auto val2 = daw::traits::is_callable_v<decltype( &f ), int>;
	static_assert( val2, "f should be callable with an int" );

	constexpr auto val3 = !daw::traits::is_callable_v<decltype( &f ), TestYesOS>;
	static_assert( val3, "f should be callable with an non-int" );

	constexpr auto val4 = !daw::traits::is_callable_v<TestYesOS, int>;
	static_assert( val4, "TestYesOS should not be callable" );

#ifdef __clang__
#ifndef __ICC // icpc defines the __clang__ macro
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif
#endif
	auto const blah2 = []( ) noexcept {
		return true;
	};
#ifdef __clang__
#ifndef __ICC // icpc defines the __clang__ macro
#pragma clang diagnostic pop
#endif
#endif

	using T = decltype( blah2 );

	constexpr bool const b1 = daw::traits::is_callable_v<T, void>;
	static_assert(
	  b1,
	  "is_callable_v<decltype( blah2 ) should be callable with a single void" );

	constexpr bool const b2 = daw::traits::is_callable_v<T>;
	static_assert(
	  b2, "is_callable_v<decltype( blah2 ) should be callable with zero args" );

	constexpr bool const b3 = daw::traits::is_callable_v<T, int>;
	static_assert( !b3, "blah2 should not be callable with an int arg" );
} // namespace daw_traits_is_callable

namespace daw_traits_has_operator {
	struct blah {
		std::string a;
	};
	static_assert( (daw::traits::operators::has_op_eq_v<int, int>),
	               "int == int failed" );
	static_assert( !(daw::traits::operators::has_op_eq_v<int, blah>),
	               "int == blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_ne_v<int, int>),
	               "int != int failed" );
	static_assert( !(daw::traits::operators::has_op_ne_v<int, blah>),
	               "int != blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_lt_v<int, int>),
	               "int < int failed" );
	static_assert( !(daw::traits::operators::has_op_lt_v<int, blah>),
	               "int < blah worked, it shoudln't" );

	static_assert( (daw::traits::operators::has_op_le_v<int, int>),
	               "int <= int failed" );
	static_assert( !(daw::traits::operators::has_op_le_v<int, blah>),
	               "int <= blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_gt_v<int, int>),
	               "int > int failed" );
	static_assert( !(daw::traits::operators::has_op_gt_v<int, blah>),
	               "int > blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_ge_v<int, int>),
	               "int >= int failed" );
	static_assert( !(daw::traits::operators::has_op_ge_v<int, blah>),
	               "int >= blah worked, it shouldn't" );
} // namespace daw_traits_has_operator

struct test_binary_pred_t {
	constexpr bool operator( )( int a, int b ) const noexcept {
		return a == b;
	}
};

namespace binary_predicate_002 {
	using a_t = daw::is_detected<daw::traits::detectors::callable_with,
	                             test_binary_pred_t, int, int>;
	constexpr auto const a = daw::is_convertible_v<a_t, bool>;
	static_assert( a, "a: Not callable with (int, int )" );

	constexpr auto const b =
	  daw::traits::is_callable_v<test_binary_pred_t, int, int>;
	static_assert( b, "b: Not callable with (int, int )" );

	constexpr auto const c =
	  daw::is_detected_convertible_v<bool, daw::traits::detectors::callable_with,
	                                 test_binary_pred_t, int, int>;
	static_assert( c, "c: Not callable with (int, int )" );

	static_assert( daw::traits::is_predicate_v<test_binary_pred_t, int, int>,
	               "Not a binary predicate" );
} // namespace binary_predicate_002

namespace is_iterator_001 {
	using iter_t = int const *;
	static_assert( daw::is_copy_constructible_v<iter_t>, "" );
	static_assert( daw::is_copy_assignable_v<iter_t>, "" );
	static_assert( daw::is_destructible_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_iterator_trait_types_v<iter_t>, "" );
	static_assert( daw::traits::is_dereferenceable_v<iter_t>, "" );
	static_assert( daw::traits::impl::is_incrementable_v<iter_t>, "" );
} // namespace is_iterator_001

namespace is_iterator_002 {
	using iter_t = int const *;
	static_assert( daw::traits::is_iterator_v<iter_t>, "" );
} // namespace is_iterator_002

namespace is_iterator_003 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::is_copy_constructible_v<iter_t>, "" );
	static_assert( daw::is_copy_assignable_v<iter_t>, "" );
	static_assert( daw::is_destructible_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_value_type_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_difference_type_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_reference_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_pointer_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_iterator_category_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_iterator_trait_types_v<iter_t>, "" );
	static_assert( daw::traits::is_dereferenceable_v<iter_t>, "" );
	static_assert( daw::traits::impl::is_incrementable_v<iter_t>, "" );
} // namespace is_iterator_003

namespace is_iterator_004 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_iterator_v<iter_t>, "" );
} // namespace is_iterator_004

namespace is_iterator_005 {
	using iter_t = int const *const;
	static_assert( daw::is_copy_constructible_v<iter_t>, "" );
	static_assert( !daw::is_copy_assignable_v<iter_t>, "" );
	static_assert( daw::is_destructible_v<iter_t>, "" );
	static_assert( !daw::traits::impl::has_iterator_trait_types_v<iter_t>, "" );
	static_assert( daw::traits::is_dereferenceable_v<iter_t>, "" );
	static_assert( !daw::traits::impl::is_incrementable_v<iter_t>, "" );
} // namespace is_iterator_005

namespace is_iterator_006 {
	using iter_t = int const *const;
	static_assert( !daw::traits::impl::is_incrementable_v<iter_t>, "" );
	static_assert( !daw::traits::is_iterator_v<iter_t>, "" );
} // namespace is_iterator_006

namespace is_iterator_007 {
	using iter_t = typename std::unordered_map<int, int>::iterator;
	static_assert( daw::traits::is_iterator<iter_t>, "" );
	static_assert( daw::is_copy_constructible_v<iter_t>, "" );
	static_assert( daw::is_copy_assignable_v<iter_t>, "" );
	static_assert( daw::is_destructible_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_value_type_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_difference_type_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_reference_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_pointer_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_iterator_category_v<iter_t>, "" );
	static_assert( daw::traits::impl::has_iterator_trait_types_v<iter_t>, "" );
	static_assert( daw::traits::is_dereferenceable_v<iter_t>, "" );
	static_assert( daw::traits::impl::is_incrementable_v<iter_t>, "" );
} // namespace is_iterator_007

namespace is_iterator_008 {
	using iter_t = typename std::unordered_map<int, int>::iterator;
	static_assert( daw::traits::is_iterator_v<iter_t>, "" );
} // namespace is_iterator_008

namespace is_random_iterator_001 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_random_access_iterator<iter_t>, "" );
} // namespace is_random_iterator_001

namespace is_random_iterator_002 {
	using iter_t = typename std::list<int>::iterator;
	static_assert( !daw::traits::is_random_access_iterator_v<iter_t>, "" );
} // namespace is_random_iterator_002

namespace is_output_iterator_001 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_output_iterator_v<iter_t, int>, "" );
} // namespace is_output_iterator_001

namespace is_output_iterator_002 {
	using iter_t = std::back_insert_iterator<std::vector<int>>;
	constexpr auto const test_value =
	  daw::traits::is_output_iterator_v<iter_t, int>;
	static_assert( test_value, "" );
} // namespace is_output_iterator_002

namespace is_output_iterator_003 {
	using iter_t = decltype( std::ostream_iterator<int>{std::cout} );
	constexpr auto const test_value =
	  daw::traits::is_output_iterator_v<iter_t, int>;
	static_assert( test_value, "" );
} // namespace is_output_iterator_003

namespace is_output_iterator_004 {
	using iter_t = decltype( std::istream_iterator<int>{std::cin} );
	static_assert( !daw::traits::is_output_iterator_v<iter_t, int>, "" );
} // namespace is_output_iterator_004

namespace is_output_iterator_005 {
	using iter_t = typename std::vector<int>::const_iterator;
	static_assert( !daw::traits::is_output_iterator_v<iter_t, int>, "" );
} // namespace is_output_iterator_005

namespace is_input_iterator_001 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_equality_comparable_v<iter_t>, "" );

	static_assert(
	  daw::is_convertible_v<decltype( *std::declval<iter_t>( ) ), int>, "" );

	static_assert( daw::traits::is_input_iterator_v<iter_t, int>, "" );
} // namespace is_input_iterator_001

namespace is_input_iterator_002 {
	using iter_t = std::back_insert_iterator<std::vector<int>>;
	static_assert( !daw::traits::is_input_iterator_v<iter_t, int>, "" );
} // namespace is_input_iterator_002

namespace is_input_iterator_003 {
	using iter_t = decltype( std::ostream_iterator<int>{std::cout} );
	static_assert( !daw::traits::is_input_iterator_v<iter_t, int>, "" );
} // namespace is_input_iterator_003

namespace is_input_iterator_004 {
	using iter_t = decltype( std::istream_iterator<int>{std::cin} );
	constexpr auto const test_value =
	  daw::traits::is_input_iterator_v<iter_t, int>;
	static_assert( test_value, "" );
} // namespace is_input_iterator_004

namespace is_input_iterator_005 {
	using iter_t = typename std::vector<int>::const_iterator;
	constexpr auto const test_value =
	  daw::traits::is_input_iterator_v<iter_t, int>;
	static_assert( test_value, "" );
} // namespace is_input_iterator_005

namespace are_convertible_to_v_001 {
	static_assert( daw::traits::are_convertible_to_v<int, char, short, unsigned>,
	               "" );
}

namespace are_convertible_to_v_002 {
	static_assert(
	  !daw::traits::are_convertible_to_v<int, char, short, unsigned, std::string>,
	  "" );
}

namespace type_n_t_test_001 {
	static_assert(
	  daw::is_same_v<int, daw::type_n_t<2, long, double, int, float>>, "" );
}

namespace if_else_t_001 {
	static_assert( daw::is_same_v<int, daw::if_else_t<true, int, std::string>>,
	               "" );
}

namespace if_else_t_002 {
	static_assert(
	  daw::is_same_v<std::string, daw::if_else_t<false, int, std::string>>, "" );
}

template<typename string_t>
constexpr bool test_string_view( ) noexcept {
	static_assert( daw::traits::has_integer_subscript_v<string_t>, "" );
	static_assert( daw::traits::has_size_memberfn_v<string_t>, "" );
	static_assert( daw::traits::has_empty_memberfn_v<string_t>, "" );
	static_assert( daw::traits::is_not_array_array_v<string_t>, "" );
	static_assert( daw::traits::is_string_view_like_v<string_t>, "" );
	return true;
}

template<typename string_t>
constexpr bool test_string( ) noexcept {
	test_string_view<string_t>( );
	static_assert( daw::traits::has_append_operator_v<string_t>, "" );
	static_assert( daw::traits::has_append_memberfn_v<string_t>, "" );
	static_assert( daw::traits::is_string_like_v<string_t>, "" );
	return true;
}

namespace string_concept_test_001 {
	constexpr bool const b = test_string<std::string>( );
}

namespace string_view_concept_test_001 {
	constexpr bool const b = test_string_view<daw::string_view>( );
}

namespace string_view_concept_test_002 {
	constexpr bool const b = test_string_view<std::string>( );
}

namespace is_member_size_equal_v {
	static_assert(
	  daw::traits::is_value_size_equal_v<std::string, sizeof( char )>, "" );
	static_assert(
	  !daw::traits::is_value_size_equal_v<std::wstring, sizeof( char )>, "" );
	static_assert(
	  daw::traits::is_value_size_equal_v<std::wstring, sizeof( wchar_t )>, "" );
} // namespace is_member_size_equal_v

namespace are_unique_test_001 {
	static_assert( daw::traits::are_unique_v<int, double, float>, "" );
}

namespace are_unique_test_002 {
	static_assert( !daw::traits::are_unique_v<int, double, int, float>, "" );
}

namespace are_unique_test_003 {
	static_assert( daw::traits::are_unique_v<int>, "" );
}

namespace isnt_cv_ref_test_001 {
	static_assert( daw::traits::isnt_cv_ref_v<int, long, char, short>, "" );
}

namespace isnt_cv_ref_test_002 {
	static_assert( !daw::traits::isnt_cv_ref_v<float, long &, char, short>, "" );
}

namespace isnt_cv_ref_test_003_ns {
	struct test_t {
		std::string s;
		int foo;
		char t;
	};

	namespace isnt_cv_ref_test_003 {
		static_assert( daw::traits::isnt_cv_ref_v<test_t, int, float, std::string>,
		               "" );
		static_assert(
		  !daw::traits::isnt_cv_ref_v<int, float, std::add_const_t<double>>, "" );
	} // namespace isnt_cv_ref_test_003
} // namespace isnt_cv_ref_test_003_ns

namespace is_first_type_test_001 {
	static_assert( daw::traits::is_first_type_v<int, int, float, double>, "" );
}

namespace is_first_type_test_002 {
	static_assert( !daw::traits::is_first_type_v<float, int, float, double>, "" );
}

namespace is_first_type_test_003 {
	static_assert( !daw::traits::is_first_type_v<float>, "" );
}

namespace is_tuple_test_001 {
	static_assert( !daw::traits::is_tuple_v<float, int>, "" );
}

namespace is_tuple_test_002 {
	static_assert( daw::traits::is_tuple_v<std::tuple<float, int>>, "" );
}

namespace is_tuple_test_003 {
	static_assert( !daw::traits::is_tuple_v<>, "" );
}

namespace is_tuple_test_004 {
	static_assert( daw::traits::is_tuple_v<std::tuple<>>, "" );
}

namespace is_callable_convertible_001 {
#ifdef __clang__
#ifndef __ICC // icpc defines the __clang__ macro
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif
#endif
	auto const func = []( auto d ) noexcept {
		return 2.0 * d;
	};
#ifdef __clang__
#ifndef __ICC // icpc defines the __clang__ macro
#pragma clang diagnostic pop
#endif
#endif
	static_assert(
	  daw::traits::is_callable_convertible_v<double, decltype( func ), double>,
	  "" );
} // namespace is_callable_convertible_001

namespace all_true_001 {
	static_assert( !daw::all_true_v<true, false, true, true, true, true, true,
	                                true, true, true, true, true, true, true>,
	               "" );
	static_assert( daw::all_true_v<true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true>,
	               "" );
} // namespace all_true_001

namespace is_init_list_constructible_001 {
	static_assert( daw::traits::is_init_list_constructible_v<std::vector<int>,
	                                                         int, int, int, int>,
	               "a vector<int> should be initializer_list constructible" );
	static_assert( !daw::traits::is_init_list_constructible_v<std::array<int, 4>,
	                                                          int, int, int, int>,
	               "a vector<int> should be initializer_list constructible" );
} // namespace is_init_list_constructible_001

namespace is_nothrow_callable_001 {
	struct X {};
	struct A {
		constexpr void operator( )( X );
		constexpr void operator( )( int ) noexcept;
	};
	static_assert( daw::traits::is_nothrow_callable_v<A, int>, "" );

	static_assert( !daw::traits::is_nothrow_callable_v<A, X>, "" );
} // namespace is_nothrow_callable_001

BOOST_AUTO_TEST_CASE( daw_traits_enable_if_any ) {
	{
		auto const result = enable_if_any_func_test( std::string( "" ) );
		BOOST_REQUIRE_MESSAGE( !result,
		                       "1. Enable if any should have defaulted to string "
		                       "only with a std::string value" );
	} // namespace daw_traits_enable_if_any)
	{
		auto const result = enable_if_any_func_test( std::string( ), true, 134,
		                                             std::string( "dfd" ) );
		BOOST_REQUIRE_MESSAGE( result,
		                       "2. Enable if any should have ran templated version "
		                       "with multiple params including a boolean" );
	}
	static_assert( enable_if_any_func_test( false ),
	               "3. Enable if any should have worked with a false" );
}

namespace is_character_001 {
	static_assert( daw::traits::is_character_v<char>, "" );
	static_assert( daw::traits::is_character_v<wchar_t>, "" );
	static_assert( !daw::traits::is_character_v<int>, "" );
} // namespace is_character_001

namespace last_type_001 {
	static_assert(
	  daw::is_same_v<daw::traits::last_type_t<int, bool, char, void, float>,
	                 float>,
	  "" );
	static_assert( daw::is_same_v<daw::traits::last_type_t<int>, int>, "" );
} // namespace last_type_001
