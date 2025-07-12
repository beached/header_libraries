// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/cpp_17.h"
#include "daw/cpp_20.h"
#include "daw/daw_benchmark.h"
#include "daw/daw_string_view.h"
#include "daw/daw_traits.h"
#include "daw/impl/daw_traits_impl.h"
#include "daw/traits/daw_traits_is_a_type.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

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
	  not daw::traits::is_equality_comparable_v<NotEqual>,
	  "3. NotEqual struct should not report as being equality comparable" );

	class NotEqual2 {
	private:
		int x;
		std::vector<int> y;

	public:
		NotEqual2( int, std::vector<int> const & )
		  : x( 0 )
		  , y( ) {
			x += 1;
		}
	};

	static_assert(
	  not daw::traits::is_equality_comparable_v<NotEqual2>,
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

	static_assert( not daw::traits::is_regular_v<NotRegular>,
	               "2. struct NotRegular should report as being regular" );
} // namespace daw_traits_is_regular

namespace daw_traits_max_sizeof {
	static_assert(
	  daw::traits::max_sizeof_v<std::int8_t, std::int16_t, std::uint8_t, int,
	                            std::int32_t, std::int64_t, std::uint64_t> ==
	    sizeof( std::uint64_t ),
	  "1. Max sizeof did not report the size of the largest item" );
}

namespace daw_traits_are_true {
	static_assert( daw::traits::are_true( true, true, 1, true ),
	               "1. are_true should have reported true" );
	static_assert( not daw::traits::are_true( true, true, false, true, 0 ),
	               "2. are_true should have reported false" );

	static_assert( not daw::traits::are_true( true, 0 ),
	               "3. are_true should have reported false" );
} // namespace daw_traits_are_true

namespace daw_traits_are_same_types {
	static_assert(
	  not daw::traits::are_same_types_v<bool, std::string, std::vector<int>>,
	  "1. Different types reported as same" );
	static_assert(
	  daw::traits::are_same_types_v<std::string, std::string, std::string>,
	  "2. Same types reported as different" );
} // namespace daw_traits_are_same_types

namespace daw_traits_bool_and {
	static_assert( daw::traits::bool_and<true, true, true>::value,
	               "1. All true's in bool_and should return true" );
	static_assert(
	  not daw::traits::bool_and<true, false, false>::value,
	  "2. A mix of true's and false's in bool_and should return false" );

	static_assert( daw::traits::bool_and<true>::value,
	               "3. A true in bool_and should return true" );
	static_assert( not daw::traits::bool_and<false>::value,
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
	static_assert( not daw::traits::bool_or<false>::value,
	               "4. A false in bool_or should return false" );

	static_assert( not daw::traits::bool_or<false, false>::value,
	               "5. Two false's in bool_or should return false" );
} // namespace daw_traits_bool_or

bool enable_if_any_func_test( std::string const &, ... ) {
	return false;
}

template<typename... Args, typename = typename daw::traits::enable_if_any<
                             bool, std::is_same<bool, Args>::value...>::type>
constexpr bool enable_if_any_func_test( Args... ) {
	return true;
}

constexpr bool enable_if_all_func_test( bool ) {
	return false;
}

template<typename... Args, typename = typename daw::traits::enable_if_all<
                             bool, std::is_same<bool, Args>::value...>::type>
constexpr bool enable_if_all_func_test( Args... ) {
	return true;
}

namespace daw_traits_enable_if_all {
	static_assert( not enable_if_all_func_test( true ),
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

	static_assert( not daw::traits::is_one_of_v<std::string, int, int, double,
	                                            std::vector<std::string>>,
	               "2. Is one of should report false when none matches" );

	static_assert(
	  daw::traits::is_one_of_v<std::string, std::string>,
	  "3. Is one of should report true with a single param and it matches" );

	static_assert( not daw::traits::is_one_of_v<std::string, int>,
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
	static_assert( not daw::traits::has_begin_member_v<T>,
	               "4. struct T should not have a begin( ) method" );
} // namespace daw_traits_has_begin_member

namespace daw_traits_has_substr_member {
	static_assert( daw::traits::has_substr_member_v<std::string>,
	               "1. std::string should have a substr method" );

	static_assert( not daw::traits::has_substr_member_v<std::vector<int>>,
	               "2. std::vector should not have a substr method" );

	using test_t =
	  std::unordered_map<std::string, int>; // Macro's and comma parameters
	static_assert( not daw::traits::has_substr_member_v<test_t>,
	               "3. std::unordered should not have a substr method" );

	struct T {
		int x;
		[[nodiscard]] constexpr bool substr( size_t, size_t ) const noexcept {
			return true;
		}
	};
	constexpr T t = { 5 };
	static_assert( t.substr( 5, 5 ) );
	static_assert( daw::traits::has_substr_member_v<T>,
	               "4. struct T should have a substr method" );
} // namespace daw_traits_has_substr_member

namespace daw_traits_has_value_type_member {
	static_assert( daw::traits::has_value_type_member_v<std::string>,
	               "1. std::string should have a value_type" );
	struct T {
		int x;
	};
	static_assert( not daw::traits::has_value_type_member_v<T>,
	               "2. T should not have a value_type" );
	static_assert( not daw::traits::has_value_type_member_v<int>,
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
	static_assert( not daw::traits::is_container_like_v<T>,
	               "4. T should not be container like" );
	static_assert( not daw::traits::is_container_like_v<int>,
	               "5. int should not be container like" );
} // namespace daw_traits_is_container_like

namespace daw_traits_is_string {
	static_assert( daw::traits::is_string_v<std::string>,
	               "1. is_string should return true for std::string" );
	static_assert(
	  not daw::traits::is_string_v<std::vector<std::string>>,
	  "2. is_string should return false for std::vector<std::string>" );
	struct T {
		int x;
	};
	static_assert( not daw::traits::is_string_v<std::vector<std::string>>,
	               "3. is_string should return false for struct T" );
	static_assert( daw::traits::is_string_v<decltype( "this is a test" )>,
	               "4. Character array should convert to string" );
	static_assert( not daw::traits::is_string_v<std::vector<char>>,
	               "5. Vector of char should not convert to string" );
} // namespace daw_traits_is_string

namespace daw_traits_isnt_string {
	static_assert( not daw::traits::isnt_string_v<std::string>,
	               "1. isnt_string should return true for std::string" );
	static_assert(
	  daw::traits::isnt_string_v<std::vector<std::string>>,
	  "2. isnt_string should return false for std::vector<std::string>" );
	struct T {
		int x;
	};
	static_assert( daw::traits::isnt_string_v<std::vector<std::string>>,
	               "3. isnt_string should return false for struct T" );
	static_assert( not daw::traits::isnt_string_v<decltype( "this is a test" )>,
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
	static_assert( not tst2, "2. TestNoOS should not have an ostream overload" );

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
	  not daw::traits::is_mixed_from_v<daw_traits_is_mixed_from_ns::Base,
	                                   daw_traits_is_mixed_from_ns::NonDerived>,
	  "2. Base<NonDerived> should not be a base for NonDerived" );
} // namespace daw_traits_is_mixed_from

constexpr int f( int ) noexcept {
	return 1;
}

namespace daw_traits_has_operator {
	struct blah {
		std::string a;
	};
	static_assert( (daw::traits::operators::has_op_eq_v<int, int>),
	               "int == int failed" );
	static_assert( not( daw::traits::operators::has_op_eq_v<int, blah> ),
	               "int == blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_ne_v<int, int>),
	               "int != int failed" );
	static_assert( not( daw::traits::operators::has_op_ne_v<int, blah> ),
	               "int != blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_lt_v<int, int>),
	               "int < int failed" );
	static_assert( not( daw::traits::operators::has_op_lt_v<int, blah> ),
	               "int < blah worked, it shoudln't" );

	static_assert( (daw::traits::operators::has_op_le_v<int, int>),
	               "int <= int failed" );
	static_assert( not( daw::traits::operators::has_op_le_v<int, blah> ),
	               "int <= blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_gt_v<int, int>),
	               "int > int failed" );
	static_assert( not( daw::traits::operators::has_op_gt_v<int, blah> ),
	               "int > blah worked, it shouldn't" );

	static_assert( (daw::traits::operators::has_op_ge_v<int, int>),
	               "int >= int failed" );
	static_assert( not( daw::traits::operators::has_op_ge_v<int, blah> ),
	               "int >= blah worked, it shouldn't" );
} // namespace daw_traits_has_operator

struct test_binary_pred_t {
	constexpr bool operator( )( int a, int b ) const noexcept {
		return a == b;
	}
};

namespace binary_predicate_002 {
	static_assert(
	  daw::traits::is_binary_predicate_v<test_binary_pred_t, int, int> );

	constexpr auto const c =
	  daw::traits::is_binary_predicate_v<test_binary_pred_t, int, int>;
	static_assert( c, "c: Not callable with (int, int )" );

	static_assert( daw::traits::is_predicate_v<test_binary_pred_t, int, int>,
	               "Not a binary predicate" );
} // namespace binary_predicate_002

namespace is_iterator_001 {
	using iter_t = int const *;
	static_assert( std::is_copy_constructible_v<iter_t> );
	static_assert( std::is_copy_assignable_v<iter_t> );
	static_assert( std::is_destructible_v<iter_t> );
	static_assert(
	  daw::traits::traits_details::has_iterator_trait_types_v<iter_t> );
	static_assert( daw::traits::is_dereferenceable_v<iter_t> );
	static_assert( daw::traits::traits_details::is_incrementable_v<iter_t> );
} // namespace is_iterator_001

namespace is_iterator_002 {
	using iter_t = int const *;
	static_assert( daw::traits::is_iterator_v<iter_t> );
} // namespace is_iterator_002

namespace is_iterator_003 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( std::is_copy_constructible_v<iter_t> );
	static_assert( std::is_copy_assignable_v<iter_t> );
	static_assert( std::is_destructible_v<iter_t> );
	static_assert( daw::traits::traits_details::has_value_type_v<iter_t> );
	static_assert( daw::traits::traits_details::has_difference_type_v<iter_t> );
	static_assert( daw::traits::traits_details::has_reference_v<iter_t> );
	static_assert( daw::traits::traits_details::has_pointer_v<iter_t> );
	static_assert( daw::traits::traits_details::has_iterator_category_v<iter_t> );
	static_assert(
	  daw::traits::traits_details::has_iterator_trait_types_v<iter_t> );
	static_assert( daw::traits::is_dereferenceable_v<iter_t> );
	static_assert( daw::traits::traits_details::is_incrementable_v<iter_t> );
} // namespace is_iterator_003

namespace is_iterator_004 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_iterator_v<iter_t> );
} // namespace is_iterator_004

namespace is_iterator_005 {
	using iter_t = int const *const;
	static_assert( std::is_copy_constructible_v<iter_t> );
	static_assert( not std::is_copy_assignable_v<iter_t> );
	static_assert( std::is_destructible_v<iter_t> );
	static_assert(
	  not daw::traits::traits_details::has_iterator_trait_types_v<iter_t> );
	static_assert( daw::traits::is_dereferenceable_v<iter_t> );
	static_assert( not daw::traits::traits_details::is_incrementable_v<iter_t> );
} // namespace is_iterator_005

namespace is_iterator_006 {
	using iter_t = int const *const;
	static_assert( not daw::traits::traits_details::is_incrementable_v<iter_t> );
	static_assert( not daw::traits::is_iterator_v<iter_t> );
} // namespace is_iterator_006

namespace is_iterator_007 {
	using iter_t = typename std::unordered_map<int, int>::iterator;
	static_assert( daw::traits::is_iterator<iter_t> );
	static_assert( std::is_copy_constructible_v<iter_t> );
	static_assert( std::is_copy_assignable_v<iter_t> );
	static_assert( std::is_destructible_v<iter_t> );
	static_assert( daw::traits::traits_details::has_value_type_v<iter_t> );
	static_assert( daw::traits::traits_details::has_difference_type_v<iter_t> );
	static_assert( daw::traits::traits_details::has_reference_v<iter_t> );
	static_assert( daw::traits::traits_details::has_pointer_v<iter_t> );
	static_assert( daw::traits::traits_details::has_iterator_category_v<iter_t> );
	static_assert(
	  daw::traits::traits_details::has_iterator_trait_types_v<iter_t> );
	static_assert( daw::traits::is_dereferenceable_v<iter_t> );
	static_assert( daw::traits::traits_details::is_incrementable_v<iter_t> );
} // namespace is_iterator_007

namespace is_iterator_008 {
	using iter_t = typename std::unordered_map<int, int>::iterator;
	static_assert( daw::traits::is_iterator_v<iter_t> );
} // namespace is_iterator_008

namespace is_random_iterator_001 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_random_access_iterator<iter_t> );
} // namespace is_random_iterator_001

namespace is_random_iterator_002 {
	using iter_t = typename std::list<int>::iterator;
	static_assert( not daw::traits::is_random_access_iterator_v<iter_t> );
} // namespace is_random_iterator_002

namespace is_output_iterator_001 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_output_iterator_v<iter_t, int> );
} // namespace is_output_iterator_001

namespace is_output_iterator_002 {
	using iter_t = std::back_insert_iterator<std::vector<int>>;
	constexpr auto const test_value =
	  daw::traits::is_output_iterator_v<iter_t, int>;
	static_assert( test_value );
} // namespace is_output_iterator_002

namespace is_output_iterator_003 {
	using iter_t = decltype( std::ostream_iterator<int>{ std::cout } );
	constexpr auto const test_value =
	  daw::traits::is_output_iterator_v<iter_t, int>;
	static_assert( test_value );
} // namespace is_output_iterator_003

namespace is_output_iterator_004 {
	using iter_t = decltype( std::istream_iterator<int>{ std::cin } );
	static_assert( not daw::traits::is_output_iterator_v<iter_t, int> );
} // namespace is_output_iterator_004

namespace is_output_iterator_005 {
	using iter_t = typename std::vector<int>::const_iterator;
	static_assert( not daw::traits::is_output_iterator_v<iter_t, int> );
} // namespace is_output_iterator_005

namespace is_input_iterator_001 {
	using iter_t = typename std::vector<int>::iterator;
	static_assert( daw::traits::is_equality_comparable_v<iter_t> );

	static_assert(
	  ::std::is_convertible_v<decltype( *std::declval<iter_t>( ) ), int> );

	static_assert( daw::traits::is_input_iterator_v<iter_t, int> );
} // namespace is_input_iterator_001

namespace is_input_iterator_002 {
	using iter_t = std::back_insert_iterator<std::vector<int>>;
	static_assert( not daw::traits::is_input_iterator_v<iter_t, int> );
} // namespace is_input_iterator_002

namespace is_input_iterator_003 {
	using iter_t = decltype( std::ostream_iterator<int>{ std::cout } );
	static_assert( not daw::traits::is_input_iterator_v<iter_t, int> );
} // namespace is_input_iterator_003

namespace is_input_iterator_004 {
	using iter_t = decltype( std::istream_iterator<int>{ std::cin } );
	constexpr auto const test_value =
	  daw::traits::is_input_iterator_v<iter_t, int>;
	static_assert( test_value );
} // namespace is_input_iterator_004

namespace is_input_iterator_005 {
	using iter_t = typename std::vector<int>::const_iterator;
	constexpr auto const test_value =
	  daw::traits::is_input_iterator_v<iter_t, int>;
	static_assert( test_value );
} // namespace is_input_iterator_005

namespace are_convertible_to_v_001 {
	static_assert(
	  daw::traits::are_convertible_to_v<int, char, short, unsigned> );
}

namespace are_convertible_to_v_002 {
	static_assert( not daw::traits::are_convertible_to_v<int, char, short,
	                                                     unsigned, std::string> );
}

namespace type_n_t_test_001 {
	static_assert(
	  std::is_same_v<int, daw::type_n_t<2, long, double, int, float>> );
}

namespace if_else_t_001 {
	static_assert( std::is_same_v<int, daw::if_else_t<true, int, std::string>> );
}

namespace if_else_t_002 {
	static_assert(
	  std::is_same_v<std::string, daw::if_else_t<false, int, std::string>> );
}

template<typename string_t>
constexpr bool test_string_view( ) noexcept {
	static_assert( daw::traits::has_integer_subscript_v<string_t> );
	static_assert( daw::traits::has_size_memberfn_v<string_t> );
	static_assert( daw::traits::has_empty_memberfn_v<string_t> );
	static_assert( daw::traits::is_not_array_array_v<string_t> );
	static_assert( daw::traits::is_string_view_like_v<string_t> );
	return true;
}

template<typename string_t>
constexpr bool test_string( ) noexcept {
	test_string_view<string_t>( );
	static_assert( daw::traits::has_append_operator_v<string_t> );
	static_assert( daw::traits::has_append_memberfn_v<string_t> );
	static_assert( daw::traits::is_string_like_v<string_t> );
	return true;
}

namespace string_concept_test_001 {
	constexpr bool const b = test_string<std::string>( );
	static_assert( b == b );
} // namespace string_concept_test_001

namespace string_view_concept_test_001 {
	constexpr bool const b = test_string_view<daw::string_view>( );
	static_assert( b == b );
} // namespace string_view_concept_test_001

namespace string_view_concept_test_002 {
	constexpr bool const b = test_string_view<std::string>( );
	static_assert( b == b );
} // namespace string_view_concept_test_002

namespace is_member_size_equal_v {
	static_assert(
	  daw::traits::is_value_size_equal_v<std::string, sizeof( char )> );
	static_assert(
	  not daw::traits::is_value_size_equal_v<std::wstring, sizeof( char )> );
	static_assert(
	  daw::traits::is_value_size_equal_v<std::wstring, sizeof( wchar_t )> );
} // namespace is_member_size_equal_v

namespace are_unique_test_001 {
	static_assert( daw::traits::are_unique_v<int, double, float> );
}

namespace are_unique_test_002 {
	static_assert( not daw::traits::are_unique_v<int, double, int, float> );
}

namespace are_unique_test_003 {
	static_assert( daw::traits::are_unique_v<int> );
}

namespace isnt_cv_ref_test_001 {
	static_assert( daw::traits::isnt_cv_ref_v<int, long, char, short> );
}

namespace isnt_cv_ref_test_002 {
	static_assert( not daw::traits::isnt_cv_ref_v<float, long &, char, short> );
}

namespace isnt_cv_ref_test_003_ns {
	struct test_t {
		std::string s;
		int foo;
		char t;
	};

	namespace isnt_cv_ref_test_003 {
		static_assert(
		  daw::traits::isnt_cv_ref_v<test_t, int, float, std::string> );
		static_assert(
		  not daw::traits::isnt_cv_ref_v<int, float, std::add_const_t<double>> );
	} // namespace isnt_cv_ref_test_003
} // namespace isnt_cv_ref_test_003_ns

namespace is_first_type_test_001 {
	static_assert( daw::traits::is_first_type_v<int, int, float, double> );
}

namespace is_first_type_test_002 {
	static_assert( not daw::traits::is_first_type_v<float, int, float, double> );
}

namespace is_first_type_test_003 {
	static_assert( not daw::traits::is_first_type_v<float> );
}

namespace is_tuple_test_001 {
	static_assert( not daw::traits::is_tuple_v<float> );
}

namespace is_tuple_test_002 {
	static_assert( daw::traits::is_tuple_v<std::tuple<float, int>> );
}

namespace is_tuple_test_004 {
	static_assert( daw::traits::is_tuple_v<std::tuple<>> );
}

namespace all_true_001 {
	static_assert(
	  not daw::all_true_v<true, false, true, true, true, true, true, true, true,
	                      true, true, true, true, true> );
	static_assert( daw::all_true_v<true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true> );
} // namespace all_true_001

namespace is_init_list_constructible_001 {
	static_assert( daw::traits::is_init_list_constructible_v<std::vector<int>,
	                                                         int, int, int, int>,
	               "a vector<int> should be initializer_list constructible" );
	static_assert( not daw::traits::is_init_list_constructible_v<
	                 std::array<int, 4>, int, int, int, int>,
	               "a vector<int> should be initializer_list constructible" );
} // namespace is_init_list_constructible_001

namespace is_nothrow_callable_001 {
	struct X {};
	struct A {
		constexpr void operator( )( X );
		constexpr void operator( )( int ) noexcept;
	};
	static_assert( daw::traits::is_nothrow_callable_v<A, int> );

	static_assert( not daw::traits::is_nothrow_callable_v<A, X> );
} // namespace is_nothrow_callable_001

void daw_traits_enable_if_any( ) {
	{
		auto const result = enable_if_any_func_test( std::string( "" ) );
		daw::expecting_message( not result,
		                        "1. Enable if any should have defaulted to string "
		                        "only with a std::string value" );
	} // namespace daw_traits_enable_if_any)
	{
		auto const result = enable_if_any_func_test( std::string( ), true, 134,
		                                             std::string( "dfd" ) );
		daw::expecting_message(
		  result,
		  "2. Enable if any should have ran templated version "
		  "with multiple params including a boolean" );
	}
	static_assert( enable_if_any_func_test( false ),
	               "3. Enable if any should have worked with a false" );
}

namespace is_character_001 {
	static_assert( daw::traits::is_character_v<char> );
	static_assert( daw::traits::is_character_v<wchar_t> );
	static_assert( not daw::traits::is_character_v<int> );
} // namespace is_character_001

namespace last_type_001 {
	static_assert(
	  std::is_same_v<daw::traits::last_type_t<int, bool, char, void, float>,
	                 float> );
	static_assert( std::is_same_v<daw::traits::last_type_t<int>, int> );
} // namespace last_type_001

static_assert( daw::traits::is_instance_of_v<std::basic_string, std::string> );
static_assert(
  daw::traits::is_instance_of_v<std::tuple, std::tuple<int, double>> );

static_assert(
  not daw::traits::is_instance_of_v<std::tuple, std::vector<int>> );
static_assert(
  not daw::traits::is_instance_of_v<std::vector, std::tuple<int, double>> );

static_assert( daw::traits::pack_index_of_v<int, double, float, int> == 2 );

struct incomplete;

static_assert( not daw::is_complete_type_v<incomplete> );
static_assert( daw::is_complete_type_v<int> );

#if defined( DAW_CPP20_CONCEPTS )
constexpr bool test( auto const * ) {
	return false;
}
constexpr bool test( daw::specialization_of<std::vector> auto const * ) {
	return true;
}
#else
template<typename T>
constexpr bool test( T const * ) {
	if( daw::is_specialization_of_v<std::vector, T> ) {
		return true;
	} else {
		return false;
	}
}
#endif
static_assert( test( static_cast<std::vector<int> const *>( nullptr ) ) );
int main( ) {
	(void)daw::expander{ 1, 2, 3 };
	daw_traits_enable_if_any( );
}

// delete_default_constructor_if<false>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_default_constructor_if<false>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_default_constructor_if<false>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_default_constructor_if<false>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_default_constructor_if<false>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_default_constructor_if<false>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_default_constructor_if<false>> );
// delete_default_constructor_if<true>
static_assert( not std::is_default_constructible_v<
               daw::traits_details::delete_default_constructor_if<true>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_default_constructor_if<true>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_default_constructor_if<true>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_default_constructor_if<true>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_default_constructor_if<true>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_default_constructor_if<true>> );

// delete_move_constructor_if<false>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_move_constructor_if<false>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_move_constructor_if<false>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_move_constructor_if<false>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_move_constructor_if<false>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_move_constructor_if<false>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_move_constructor_if<false>> );
// delete_move_constructor_if<true>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_move_constructor_if<true>> );
static_assert( not std::is_move_constructible_v<
               daw::traits_details::delete_move_constructor_if<true>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_move_constructor_if<true>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_move_constructor_if<true>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_move_constructor_if<true>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_move_constructor_if<true>> );
// delete_move_assignment_if<false>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_move_assignment_if<false>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_move_assignment_if<false>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_move_assignment_if<false>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_move_assignment_if<false>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_move_assignment_if<false>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_move_assignment_if<false>> );
// delete_move_assignment_if<true>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_move_assignment_if<true>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_move_assignment_if<true>> );
static_assert( not std::is_move_assignable_v<
               daw::traits_details::delete_move_assignment_if<true>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_move_assignment_if<true>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_move_assignment_if<true>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_move_assignment_if<true>> );
// delete_copy_constructor_if<false>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_copy_constructor_if<false>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_copy_constructor_if<false>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_copy_constructor_if<false>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_copy_constructor_if<false>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_copy_constructor_if<false>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_copy_constructor_if<false>> );
// delete_copy_constructor_if<true>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_copy_constructor_if<true>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_copy_constructor_if<true>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_copy_constructor_if<true>> );
static_assert( not std::is_copy_constructible_v<
               daw::traits_details::delete_copy_constructor_if<true>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_copy_constructor_if<true>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_copy_constructor_if<true>> );
// delete_copy_assignment_if<false>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_copy_assignment_if<false>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_copy_assignment_if<false>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_copy_assignment_if<false>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_copy_assignment_if<false>> );
static_assert( std::is_trivially_copy_assignable_v<
               daw::traits_details::delete_copy_assignment_if<false>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_copy_assignment_if<false>> );
// delete_copy_assignment_if<true>
static_assert( std::is_trivially_default_constructible_v<
               daw::traits_details::delete_copy_assignment_if<true>> );
static_assert( std::is_trivially_move_constructible_v<
               daw::traits_details::delete_copy_assignment_if<true>> );
static_assert( std::is_trivially_move_assignable_v<
               daw::traits_details::delete_copy_assignment_if<true>> );
static_assert( std::is_trivially_copy_constructible_v<
               daw::traits_details::delete_copy_assignment_if<true>> );
static_assert( not std::is_copy_assignable_v<
               daw::traits_details::delete_copy_assignment_if<true>> );
static_assert( std::is_trivially_destructible_v<
               daw::traits_details::delete_copy_assignment_if<true>> );
