// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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
#include <string>
#include <unordered_map>
#include <vector>

#include "boost_test.h"
#include "daw_string_view.h"
#include "daw_traits.h"

BOOST_AUTO_TEST_CASE( daw_traits_is_equality_comparable ) {
	BOOST_REQUIRE_MESSAGE( daw::is_equality_comparable_v<std::string>,
	                       "1. std::string should report as being equality comparable" );
	BOOST_REQUIRE_MESSAGE( daw::is_equality_comparable_v<std::vector<std::string>>,
	                       "2. std::vector should report as being equality comparable" );
	{
		struct NotEqual {
			int x;
		};
		BOOST_REQUIRE_MESSAGE( !daw::is_equality_comparable_v<NotEqual>,
		                       "3. NotEqual struct should not report as being equality comparable" );
	}
	{
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
		NotEqual2 ne2( 2, {2, 2} );
		BOOST_REQUIRE_MESSAGE( !daw::is_equality_comparable_v<NotEqual2>,
		                       "4. NotEqual2 struct should not report as being equality comparable" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_is_regular ) {
	BOOST_REQUIRE_MESSAGE( daw::is_regular_v<std::string>, "1. std::string should report as being regular" );
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
	NotRegular x{5};
	BOOST_REQUIRE_MESSAGE( !daw::is_regular_v<NotRegular>, "2. struct NotRegular should report as being regular" );
}

BOOST_AUTO_TEST_CASE( daw_traits_max_sizeof ) {
	auto result = daw::traits::max_sizeof<int8_t, int16_t, uint8_t, int, int32_t, int64_t, uint64_t, size_t>::value;
	BOOST_REQUIRE_MESSAGE( result == sizeof( size_t ), "1. Max sizeof did not report the size of the largest item" );
}

BOOST_AUTO_TEST_CASE( daw_traits_are_true ) {
	{
		auto result = daw::traits::are_true( true, true, 1, true );
		BOOST_REQUIRE_MESSAGE( result, "1. are_true should have reported true" );
	}
	{
		auto result = daw::traits::are_true( true, true, false, true, 0 );
		BOOST_REQUIRE_MESSAGE( !result, "2. are_true should have reported false" );
	}

	{
		auto result = daw::traits::are_true( true, 0 );
		BOOST_REQUIRE_MESSAGE( !result, "3. are_true should have reported false" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_are_same_types ) {
	{
		auto result = daw::traits::are_same_types_v<bool, std::string, std::vector<int>>;
		BOOST_REQUIRE_MESSAGE( !result, "1. Different types reported as same" );
	}
	{
		auto result = daw::traits::are_same_types_v<std::string, std::string, std::string>;
		BOOST_REQUIRE_MESSAGE( result, "2. Same types reported as different" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_bool_and ) {
	{
		auto result = daw::traits::bool_and<true, true, true>::value;
		BOOST_REQUIRE_MESSAGE( result, "1. All true's in bool_and should return true" );
	}
	{
		auto result = daw::traits::bool_and<true, false, false>::value;
		BOOST_REQUIRE_MESSAGE( !result, "2. A mix of true's and false's in bool_and should return false" );
	}
	{
		auto result = daw::traits::bool_and<true>::value;
		BOOST_REQUIRE_MESSAGE( result, "3. A true in bool_and should return true" );
	}
	{
		auto result = daw::traits::bool_and<false>::value;
		BOOST_REQUIRE_MESSAGE( !result, "4. A false in bool_and should return false" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_bool_or ) {
	{
		auto result = daw::traits::bool_or<true, true, true>::value;
		BOOST_REQUIRE_MESSAGE( result, "1. All true's in bool_or should return true" );
	}
	{
		auto result = daw::traits::bool_or<true, false, false>::value;
		BOOST_REQUIRE_MESSAGE( result, "2. A mix of true's and false's in bool_or should return true" );
	}
	{
		auto result = daw::traits::bool_or<true>::value;
		BOOST_REQUIRE_MESSAGE( result, "3. A true in bool_or should return true" );
	}
	{ // Seems to fail in MSVC 2013, not clang++ or g++
		auto result = daw::traits::bool_or<false>::value;
		BOOST_REQUIRE_MESSAGE( !result, "4. A false in bool_or should return false" );
	}
	{
		auto result = daw::traits::bool_or<false, false>::value;
		BOOST_REQUIRE_MESSAGE( !result, "5. Two false's in bool_or should return false" );
	}
}

bool enable_if_any_func_test( std::string, ... ) {
	return false;
}

template<typename... Args,
         typename = typename daw::traits::enable_if_any<bool, std::is_same<bool, Args>::value...>::type>
bool enable_if_any_func_test( Args... args ) {
	return true;
}

BOOST_AUTO_TEST_CASE( daw_traits_enable_if_any ) {
	{
		auto result = enable_if_any_func_test( std::string( "" ) );
		BOOST_REQUIRE_MESSAGE( !result, "1. Enable if any should have defaulted to string only with a std::string value" );
	}
	{
		auto result = enable_if_any_func_test( std::string( ), true, 134, std::string( "dfd" ) );
		BOOST_REQUIRE_MESSAGE(
		  result, "2. Enable if any should have ran templated version with multiple params including a boolean" );
	}
	{
		auto result = enable_if_any_func_test( false );
		BOOST_REQUIRE_MESSAGE( result, "3. Enable if any should have worked with a false" );
	}
}

bool enable_if_all_func_test( bool ) {
	return false;
}

template<typename... Args,
         typename = typename daw::traits::enable_if_all<bool, std::is_same<bool, Args>::value...>::type>
bool enable_if_all_func_test( Args... args ) {
	return true;
}

BOOST_AUTO_TEST_CASE( daw_traits_enable_if_all ) {
	{
		auto result = enable_if_all_func_test( true );
		BOOST_REQUIRE_MESSAGE( !result, "1. Enable if all should have defaulted to string only with a std::string value" );
	}
	{
		auto result = enable_if_all_func_test( true, true, true, true, true );
		BOOST_REQUIRE_MESSAGE( result,
		                       "2. Enable if all should have ran templated version with multiple "
		                       "params including a boolean but failed and defaulted to non-templated "
		                       "version" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_is_one_of ) {
	{
		auto result = daw::traits::is_one_of_v<std::string, std::string, std::string, int, std::vector<std::string>>;
		BOOST_REQUIRE_MESSAGE( result, "1. Is one of should report true when at least one matches" );
	}
	{
		auto result = daw::traits::is_one_of_v<std::string, int, int, double, std::vector<std::string>>;
		BOOST_REQUIRE_MESSAGE( !result, "2. Is one of should report false when none matches" );
	}
	{
		auto result = daw::traits::is_one_of_v<std::string, std::string>;
		BOOST_REQUIRE_MESSAGE( result, "3. Is one of should report true with a single param and it matches" );
	}
	{
		auto result = daw::traits::is_one_of_v<std::string, int>;
		BOOST_REQUIRE_MESSAGE( !result, "4. Is one of should report false with a single param and it does not match" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_has_begin_member ) {
	BOOST_REQUIRE_MESSAGE( daw::traits::has_begin_member_v<std::string>, "1. std::string should have a begin( ) method" );

	BOOST_REQUIRE_MESSAGE( daw::traits::has_begin_member_v<std::vector<int>>,
	                       "2. std::vector should have a begin( ) method" );
	{
		using test_t = std::unordered_map<std::string, int>; // Macro's and comma parameters
		BOOST_REQUIRE_MESSAGE( daw::traits::has_begin_member_v<test_t>, "3. std::unordered should have a begin( ) method" );
	}
	{
		struct T {
			int x;
		};
		BOOST_REQUIRE_MESSAGE( !daw::traits::has_begin_member_v<T>, "4. struct T should not have a begin( ) method" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_has_substr_member ) {
	BOOST_REQUIRE_MESSAGE( daw::traits::has_substr_member_v<std::string>, "1. std::string should have a substr method" );

	BOOST_REQUIRE_MESSAGE( !daw::traits::has_substr_member_v<std::vector<int>>,
	                       "2. std::vector should not have a substr method" );
	{
		using test_t = std::unordered_map<std::string, int>; // Macro's and comma parameters
		BOOST_REQUIRE_MESSAGE( !daw::traits::has_substr_member_v<test_t>,
		                       "3. std::unordered should not have a substr method" );
	}
	{
		struct T {
			int x;
			bool substr( size_t, size_t ) {
				return true;
			}
		};
		T t = {5};
		t.substr( 5, 5 );
		BOOST_REQUIRE_MESSAGE( daw::traits::has_substr_member_v<T>, "4. struct T should have a substr method" );
	}
}

BOOST_AUTO_TEST_CASE( daw_traits_has_value_type_member ) {
	BOOST_REQUIRE_MESSAGE( daw::traits::has_value_type_member_v<std::string>, "1. std::string should have a value_type" );
	struct T {
		int x;
	};
	BOOST_REQUIRE_MESSAGE( !daw::traits::has_value_type_member_v<T>, "2. T should not have a value_type" );
	BOOST_REQUIRE_MESSAGE( !daw::traits::has_value_type_member_v<int>, "3. int should not have a value_type" );
}

BOOST_AUTO_TEST_CASE( daw_traits_is_container_like ) {
	BOOST_REQUIRE_MESSAGE( daw::traits::is_container_like_v<std::string>, "1. std::string should be container like" );
	BOOST_REQUIRE_MESSAGE( daw::traits::is_container_like_v<std::vector<std::string>>,
	                       "2. std::vector<std::string> should be container like" );
	using map_t = std::unordered_map<std::string, int>;
	BOOST_REQUIRE_MESSAGE( daw::traits::is_container_like_v<map_t>,
	                       "3. std::unordered_map<std::string, int> should be container like" );
	struct T {
		int x;
	};
	BOOST_REQUIRE_MESSAGE( !daw::traits::is_container_like_v<T>, "4. T should not be container like" );
	BOOST_REQUIRE_MESSAGE( !daw::traits::is_container_like_v<int>, "5. int should not be container like" );
}

BOOST_AUTO_TEST_CASE( daw_traits_is_string ) {
	BOOST_REQUIRE_MESSAGE( daw::traits::is_string_v<std::string>, "1. is_string should return true for std::string" );
	BOOST_REQUIRE_MESSAGE( !daw::traits::is_string_v<std::vector<std::string>>,
	                       "2. is_string should return false for std::vector<std::string>" );
	struct T {
		int x;
	};
	BOOST_REQUIRE_MESSAGE( !daw::traits::is_string_v<std::vector<std::string>>,
	                       "3. is_string should return false for struct T" );
	BOOST_REQUIRE_MESSAGE( daw::traits::is_string_v<decltype( "this is a test" )>,
	                       "4. Character array should convert to string" );
	BOOST_REQUIRE_MESSAGE( !daw::traits::is_string_v<std::vector<char>>,
	                       "5. Vector of char should not convert to string" );
}

BOOST_AUTO_TEST_CASE( daw_traits_isnt_string ) {
	BOOST_REQUIRE_MESSAGE( !daw::traits::isnt_string_v<std::string>,
	                       "1. isnt_string should return true for std::string" );
	BOOST_REQUIRE_MESSAGE( daw::traits::isnt_string_v<std::vector<std::string>>,
	                       "2. isnt_string should return false for std::vector<std::string>" );
	struct T {
		int x;
	};
	BOOST_REQUIRE_MESSAGE( daw::traits::isnt_string_v<std::vector<std::string>>,
	                       "3. isnt_string should return false for struct T" );
	BOOST_REQUIRE_MESSAGE( !daw::traits::isnt_string_v<decltype( "this is a test" )>,
	                       "4. Character array should convert to string" );
	BOOST_REQUIRE_MESSAGE( daw::traits::isnt_string_v<std::vector<char>>,
	                       "5. Vector of char should not convert to string" );
}

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

BOOST_AUTO_TEST_CASE( daw_traits_is_streamable ) {
	BOOST_REQUIRE_MESSAGE( daw::traits::is_streamable_v<int>, "1. int should have an ostream overload" );

	BOOST_REQUIRE_MESSAGE( !daw::traits::is_streamable_v<TestNoOS>, "2. TestNoOS should not have an ostream overload" );

	BOOST_REQUIRE_MESSAGE( daw::traits::is_streamable_v<TestYesOS>, "3. TestYesOS should have an ostream overload" );
}

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
BOOST_AUTO_TEST_CASE( daw_traits_is_mixed_from ) {
	auto test1 = daw::traits::is_mixed_from_v<daw_traits_is_mixed_from_ns::Base, daw_traits_is_mixed_from_ns::Derived>;
	BOOST_REQUIRE_MESSAGE( test1, "1. Base<Child> should be a base for Child" );

	auto test2 = daw::traits::is_mixed_from_v<daw_traits_is_mixed_from_ns::Base, daw_traits_is_mixed_from_ns::NonDerived>;
	BOOST_REQUIRE_MESSAGE( !test2, "2. Base<NonDerived> should not be a base for NonDerived" );
}

int f( int ) {
	return 1;
}
/*
BOOST_AUTO_TEST_CASE( daw_traits_is_callable ) {
    auto blah = []( auto t ) { return t; };
    bool val = daw::traits::is_callable_v<decltype(blah), int>;
    BOOST_REQUIRE_MESSAGE( val, "blah should be callable" );
    val = daw::traits::is_callable_v<decltype(&f), int>;
    BOOST_REQUIRE_MESSAGE( val, "f should be callable with an int" );
    val = !daw::traits::is_callable_v<decltype(&f), TestYesOS>;
    BOOST_REQUIRE_MESSAGE( val, "f should be callable with an non-int" );
    val = !daw::traits::is_callable_v<TestYesOS, int>;
    BOOST_REQUIRE_MESSAGE( val , "TestYesOS should not be callable" );

}
*/
BOOST_AUTO_TEST_CASE( daw_traits_has_operator ) {
	struct blah {
		std::string a;
	};
	BOOST_REQUIRE( (daw::traits::operators::has_op_eq_v<int, int>));
	BOOST_REQUIRE( !(daw::traits::operators::has_op_eq_v<int, blah>));

	BOOST_REQUIRE( (daw::traits::operators::has_op_ne_v<int, int>));
	BOOST_REQUIRE( !(daw::traits::operators::has_op_ne_v<int, blah>));

	BOOST_REQUIRE( (daw::traits::operators::has_op_lt_v<int, int>));
	BOOST_REQUIRE( !(daw::traits::operators::has_op_lt_v<int, blah>));

	BOOST_REQUIRE( (daw::traits::operators::has_op_le_v<int, int>));
	BOOST_REQUIRE( !(daw::traits::operators::has_op_le_v<int, blah>));

	BOOST_REQUIRE( (daw::traits::operators::has_op_gt_v<int, int>));
	BOOST_REQUIRE( !(daw::traits::operators::has_op_gt_v<int, blah>));

	BOOST_REQUIRE( (daw::traits::operators::has_op_ge_v<int, int>));
	BOOST_REQUIRE( !(daw::traits::operators::has_op_ge_v<int, blah>));
}

struct test_binary_pred_t {
	constexpr bool operator( )( int a, int b ) const noexcept {
		return a == b;
	}
};

BOOST_AUTO_TEST_CASE( binary_predicate_002 ) {
	using a_t = daw::is_detected<daw::detectors::callable_with, test_binary_pred_t, int, int>;
	using a_conv_t = std::is_convertible<a_t, bool>;
	constexpr auto const a = a_conv_t::value;
	static_assert( a, "a: Not callable with (int, int )" );

	constexpr auto const b = daw::is_callable_v<test_binary_pred_t, int, int>;
	static_assert( b, "b: Not callable with (int, int )" );

	constexpr auto const c =
	  daw::is_detected_convertible_v<bool, daw::detectors::callable_with, test_binary_pred_t, int, int>;
	static_assert( c, "c: Not callable with (int, int )" );

	static_assert( daw::is_predicate_v<test_binary_pred_t, int, int>, "Not a binary predicate" );
	BOOST_REQUIRE( a );
}

BOOST_AUTO_TEST_CASE( is_iterator_001 ) {
	using iter_t = int const *;
	BOOST_REQUIRE( daw::is_copy_constructible_v<iter_t> );
	BOOST_REQUIRE( daw::is_copy_assignable_v<iter_t> );
	BOOST_REQUIRE( daw::is_destructible_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_iterator_trait_types_v<iter_t> );
	BOOST_REQUIRE( daw::is_dereferenceable_v<iter_t> );
	BOOST_REQUIRE( daw::impl::is_incrementable_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_iterator_002 ) {
	using iter_t = int const *;
	BOOST_REQUIRE( daw::is_iterator_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_iterator_003 ) {
	using iter_t = typename std::vector<int>::iterator;
	BOOST_REQUIRE( daw::is_copy_constructible_v<iter_t> );
	BOOST_REQUIRE( daw::is_copy_assignable_v<iter_t> );
	BOOST_REQUIRE( daw::is_destructible_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_value_type_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_difference_type_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_reference_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_pointer_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_iterator_category_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_iterator_trait_types_v<iter_t> );
	BOOST_REQUIRE( daw::is_dereferenceable_v<iter_t> );
	BOOST_REQUIRE( daw::impl::is_incrementable_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_iterator_004 ) {
	using iter_t = typename std::vector<int>::iterator;
	BOOST_REQUIRE( daw::is_iterator_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_iterator_005 ) {
	using iter_t = int const *const;
	BOOST_REQUIRE( daw::is_copy_constructible_v<iter_t> );
	BOOST_REQUIRE( !daw::is_copy_assignable_v<iter_t> );
	BOOST_REQUIRE( daw::is_destructible_v<iter_t> );
	BOOST_REQUIRE( !daw::impl::has_iterator_trait_types_v<iter_t> );
	BOOST_REQUIRE( daw::is_dereferenceable_v<iter_t> );
	BOOST_REQUIRE( !daw::impl::is_incrementable_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_iterator_006 ) {
	using iter_t = int const *const;
	BOOST_REQUIRE( !daw::is_iterator_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_iterator_007 ) {
	using iter_t = typename std::unordered_map<int, int>::iterator;
	BOOST_REQUIRE( daw::is_copy_constructible_v<iter_t> );
	BOOST_REQUIRE( daw::is_copy_assignable_v<iter_t> );
	BOOST_REQUIRE( daw::is_destructible_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_value_type_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_difference_type_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_reference_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_pointer_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_iterator_category_v<iter_t> );
	BOOST_REQUIRE( daw::impl::has_iterator_trait_types_v<iter_t> );
	BOOST_REQUIRE( daw::is_dereferenceable_v<iter_t> );
	BOOST_REQUIRE( daw::impl::is_incrementable_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_iterator_008 ) {
	using iter_t = typename std::unordered_map<int, int>::iterator;
	BOOST_REQUIRE( daw::is_iterator_v<iter_t> );
}

BOOST_AUTO_TEST_CASE( is_output_iterator_001 ) {
	using iter_t = typename std::vector<int>::iterator;
	constexpr auto const test_value = daw::is_output_iterator_v<iter_t, int>;
	BOOST_REQUIRE( test_value );
}

BOOST_AUTO_TEST_CASE( is_output_iterator_002 ) {
	using iter_t = std::back_insert_iterator<std::vector<int>>;
	constexpr auto const test_value = daw::is_output_iterator_v<iter_t, int>;
	BOOST_REQUIRE( test_value );
}

BOOST_AUTO_TEST_CASE( is_output_iterator_003 ) {
	using iter_t = decltype( std::ostream_iterator<int>{std::cout} );
	constexpr auto const test_value = daw::is_output_iterator_v<iter_t, int>;
	BOOST_REQUIRE( test_value );
}

BOOST_AUTO_TEST_CASE( is_output_iterator_004 ) {
	using iter_t = decltype( std::istream_iterator<int>{std::cin} );
	constexpr auto const test_value = daw::is_output_iterator_v<iter_t, int>;
	BOOST_REQUIRE( !test_value );
}

BOOST_AUTO_TEST_CASE( is_output_iterator_005 ) {
	using iter_t = typename std::vector<int>::const_iterator;
	constexpr auto const test_value = daw::is_output_iterator_v<iter_t, int>;
	BOOST_REQUIRE( !test_value );
}

BOOST_AUTO_TEST_CASE( is_input_iterator_001 ) {
	using iter_t = typename std::vector<int>::iterator;
	{
		constexpr auto const test_value = daw::is_equality_comparable_v<iter_t>;
		BOOST_REQUIRE( test_value );
	}
	{
		constexpr auto const test_value = daw::is_convertible_v<decltype( *std::declval<iter_t>( ) ), int>;
		BOOST_REQUIRE( test_value );
	}
	{
		constexpr auto const test_value = daw::is_input_iterator_v<iter_t, int>;
		BOOST_REQUIRE( test_value );
	}
}

BOOST_AUTO_TEST_CASE( is_input_iterator_002 ) {
	using iter_t = std::back_insert_iterator<std::vector<int>>;
	constexpr auto const test_value = daw::is_input_iterator_v<iter_t, int>;
	BOOST_REQUIRE( !test_value );
}

BOOST_AUTO_TEST_CASE( is_input_iterator_003 ) {
	using iter_t = decltype( std::ostream_iterator<int>{std::cout} );
	constexpr auto const test_value = daw::is_input_iterator_v<iter_t, int>;
	BOOST_REQUIRE( !test_value );
}

BOOST_AUTO_TEST_CASE( is_input_iterator_004 ) {
	using iter_t = decltype( std::istream_iterator<int>{std::cin} );
	constexpr auto const test_value = daw::is_input_iterator_v<iter_t, int>;
	BOOST_REQUIRE( test_value );
}

BOOST_AUTO_TEST_CASE( is_input_iterator_005 ) {
	using iter_t = typename std::vector<int>::const_iterator;
	constexpr auto const test_value = daw::is_input_iterator_v<iter_t, int>;
	BOOST_REQUIRE( test_value );
}

BOOST_AUTO_TEST_CASE( are_convertible_to_v_001 ) {
	constexpr bool const a = daw::are_convertible_to_v<int, char, short, unsigned>;
	BOOST_REQUIRE( a );
}

BOOST_AUTO_TEST_CASE( are_convertible_to_v_002 ) {
	constexpr bool const a = daw::are_convertible_to_v<int, char, short, unsigned, std::string>;
	BOOST_REQUIRE( !a );
}

BOOST_AUTO_TEST_CASE( type_n_t_test_001 ) {
	constexpr bool tuple_n_t_test = daw::is_same_v<int, daw::type_n_t<2, long, double, int, float>>;
	BOOST_REQUIRE( tuple_n_t_test );
}

BOOST_AUTO_TEST_CASE( if_else_t_001 ) {
	constexpr bool tst = daw::is_same_v<int, daw::if_else_t<true, int, std::string>>;
	BOOST_REQUIRE( tst );
}

BOOST_AUTO_TEST_CASE( if_else_t_002 ) {
	constexpr bool tst = daw::is_same_v<std::string, daw::if_else_t<false, int, std::string>>;
	BOOST_REQUIRE( tst );
}

template<typename string_t>
void test_string_view( ) {
	constexpr bool has_integer_subscript = daw::traits::has_integer_subscript_v<string_t>;
	constexpr bool has_size_memberfn = daw::traits::has_size_memberfn_v<string_t>;
	constexpr bool has_empty_memberfn = daw::traits::has_empty_memberfn_v<string_t>;
	constexpr bool is_not_array_array = daw::traits::is_not_array_array_v<string_t>;
	constexpr bool is_string_view_like = daw::traits::is_string_view_like_v<string_t>;

	BOOST_REQUIRE( has_integer_subscript );
	BOOST_REQUIRE( has_size_memberfn );
	BOOST_REQUIRE( has_empty_memberfn );
	BOOST_REQUIRE( is_not_array_array );
	BOOST_REQUIRE( is_string_view_like );
}

template<typename string_t>
void test_string( ) {
	test_string_view<string_t>( );
	constexpr bool has_append_operator = daw::traits::has_append_operator_v<string_t>;
	constexpr bool has_append_memberfn = daw::traits::has_append_memberfn_v<string_t>;
	constexpr bool is_string_like = daw::traits::is_string_like_v<string_t>;

	BOOST_REQUIRE( has_append_operator );
	BOOST_REQUIRE( has_append_memberfn );
	BOOST_REQUIRE( is_string_like );
}

BOOST_AUTO_TEST_CASE( string_concept_test_001 ) {
	test_string<std::string>( );
}

BOOST_AUTO_TEST_CASE( string_view_concept_test_001 ) {
	test_string_view<daw::string_view>( );
}

BOOST_AUTO_TEST_CASE( string_view_concept_test_002 ) {
	test_string_view<std::string>( );
}

BOOST_AUTO_TEST_CASE( is_member_size_equal_v ) {
	constexpr bool result_t = daw::traits::is_value_size_equal_v<std::string, sizeof( char )>;
	constexpr bool result_f = daw::traits::is_value_size_equal_v<std::wstring, sizeof( char )>;
	constexpr bool result_t2 = daw::traits::is_value_size_equal_v<std::wstring, sizeof( wchar_t )>;
	BOOST_REQUIRE( result_t );
	BOOST_REQUIRE( !result_f );
	BOOST_REQUIRE( result_t2 );
}

BOOST_AUTO_TEST_CASE( are_unique_test_001 ) {
	constexpr bool t1 = daw::traits::are_unique_v<int, double, float>;
	BOOST_REQUIRE( t1 );
}

BOOST_AUTO_TEST_CASE( are_unique_test_002 ) {
	constexpr bool t1 = daw::traits::are_unique_v<int, double, int, float>;
	BOOST_REQUIRE( !t1 );
}

BOOST_AUTO_TEST_CASE( are_unique_test_003 ) {
	constexpr bool t1 = daw::traits::are_unique_v<int>;
	BOOST_REQUIRE( t1 );
}

BOOST_AUTO_TEST_CASE( isnt_cv_ref_test_001 ) {
	constexpr bool t1 = daw::traits::isnt_cv_ref_v<int, long, char, short>;
	BOOST_REQUIRE( t1 );
}

BOOST_AUTO_TEST_CASE( isnt_cv_ref_test_002 ) {
	constexpr bool t1 = daw::traits::isnt_cv_ref_v<float, long &, char, short>;
	BOOST_REQUIRE( !t1 );
}

namespace isnt_cv_ref_test_003_ns {
	struct test_t {
		std::string s;
		int foo;
		char t;
	};

	BOOST_AUTO_TEST_CASE( isnt_cv_ref_test_003 ) {
		constexpr bool t1 = daw::traits::isnt_cv_ref_v<test_t, int, float, std::string>;
		BOOST_REQUIRE( t1 );
		constexpr bool t2 = daw::traits::isnt_cv_ref_v<int, float, std::add_const_t<double>>;
		BOOST_REQUIRE( !t2 );
	}
} // namespace isnt_cv_ref_test_003_ns
