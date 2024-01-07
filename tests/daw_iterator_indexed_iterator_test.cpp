// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/cpp_17.h"
#include "daw/cpp_17_iterator.h"
#include "daw/iterator/daw_indexed_iterator.h"

#include <array>

namespace {
	constexpr bool array_test_001( ) noexcept {
		std::array<int, 7> numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		return *f == 1;
	}

	static_assert( array_test_001( ) );

	constexpr bool array_test_002( ) noexcept {
		std::array<int, 7> numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );
		return daw::distance( f, l ) == 7;
	}

	static_assert( array_test_002( ) );

	constexpr bool array_test_003( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		return *f == 1;
	}

	static_assert( array_test_003( ) );

	constexpr bool array_test_004( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );
		return daw::distance( f, l ) == 7;
	}

	static_assert( array_test_004( ) );

	constexpr bool array_test_005( ) noexcept {
		std::array<int, 7> numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::cibegin( numbers );
		return *f == 1;
	}

	static_assert( array_test_005( ) );

	constexpr bool array_test_006( ) noexcept {
		std::array<int, 7> numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::cibegin( numbers );
		auto l = daw::ciend( numbers );
		return daw::distance( f, l ) == 7;
	}

	static_assert( array_test_006( ) );

	constexpr bool array_test_007( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::cibegin( numbers );
		return *f == 1;
	}

	static_assert( array_test_007( ) );

	constexpr bool array_test_008( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::cibegin( numbers );
		auto l = daw::ciend( numbers );
		return daw::distance( f, l ) == 7;
	}

	static_assert( array_test_008( ) );

	constexpr bool inc_test_001( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto it = daw::cibegin( numbers );
		++it;
		return *it == 2;
	}

	static_assert( inc_test_001( ) );

	constexpr bool inc_test_002( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto it = daw::cibegin( numbers );
		it++;
		return *it == 2;
	}

	static_assert( inc_test_002( ) );

	constexpr bool inc_test_003( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto it = daw::cibegin( numbers );
		it += 2;
		return *it == 3;
	}

	static_assert( inc_test_003( ) );

	constexpr bool dec_test_001( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto it = daw::ciend( numbers );
		--it;
		return *it == 7;
	}

	static_assert( dec_test_001( ) );

	constexpr bool dec_test_002( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto it = daw::ciend( numbers );
		it--;
		return *it == 7;
	}

	static_assert( dec_test_002( ) );

	constexpr bool dec_test_003( ) noexcept {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto it = daw::ciend( numbers );
		it -= 2;
		return *it == 6;
	}

	static_assert( dec_test_003( ) );

	struct A {
		constexpr bool t( ) const {
			return true;
		}
	};

	constexpr bool operator_arrow_001( ) noexcept {
		std::array<A, 5> as = { A{ }, A{ }, A{ }, A{ }, A{} };
		auto it = daw::ibegin( as );

		return it->t( );
	}

	static_assert( operator_arrow_001( ) );

	constexpr bool operator_arrow_002( ) noexcept {
		std::array<A, 5> const as = { A{ }, A{ }, A{ }, A{ }, A{} };
		auto it = daw::cibegin( as );

		return it->t( );
	}

	static_assert( operator_arrow_002( ) );

	constexpr bool carray_ibegin_001( ) noexcept {
		int numbers[3] = { 1, 2, 3 };
		return *daw::ibegin( numbers ) == 1;
	}
	static_assert( carray_ibegin_001( ) );

	constexpr bool carray_ibegin_002( ) noexcept {
		int const numbers[3] = { 1, 2, 3 };
		return *daw::ibegin( numbers ) == 1;
	}
	static_assert( carray_ibegin_002( ) );

	constexpr bool carray_cibegin_001( ) noexcept {
		int numbers[3] = { 1, 2, 3 };
		return *daw::cibegin( numbers ) == 1;
	}
	static_assert( carray_cibegin_001( ) );

	constexpr bool carray_cibegin_002( ) noexcept {
		int const numbers[3] = { 1, 2, 3 };
		return *daw::cibegin( numbers ) == 1;
	}
	static_assert( carray_cibegin_002( ) );

	constexpr bool carray_iend_001( ) noexcept {
		int numbers[3] = { 1, 2, 3 };
		auto it = daw::iend( numbers );
		auto it2 = daw::ibegin( numbers ) + 3;

		return it == it2;
	}
	static_assert( carray_iend_001( ) );

	constexpr bool carray_iend_002( ) noexcept {
		int const numbers[3] = { 1, 2, 3 };
		auto it = daw::iend( numbers );
		auto it2 = daw::ibegin( numbers ) + 3;

		return it == it2;
	}
	static_assert( carray_iend_002( ) );

	constexpr bool carray_ciend_001( ) noexcept {
		int numbers[3] = { 1, 2, 3 };
		auto it = daw::ciend( numbers );
		auto it2 = daw::cibegin( numbers ) + 3;

		return it == it2;
	}
	static_assert( carray_ciend_001( ) );

	constexpr bool carray_ciend_002( ) noexcept {
		int const numbers[3] = { 1, 2, 3 };
		auto it = daw::ciend( numbers );
		auto it2 = daw::cibegin( numbers ) + 3;

		return it == it2;
	}
	static_assert( carray_ciend_002( ) );

	constexpr bool operator_equal_001( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers ) - numbers.size( );

		return f == l;
	}
	static_assert( operator_equal_001( ) );

	constexpr bool operator_not_equal_001( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );

		return f != l;
	}
	static_assert( operator_not_equal_001( ) );

	constexpr bool operator_less_001( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );

		return f < l;
	}
	static_assert( operator_less_001( ) );

	constexpr bool operator_less_equal_001( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );

		return f <= l;
	}
	static_assert( operator_less_equal_001( ) );

	constexpr bool operator_less_equal_002( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );

		return f <= f;
	}
	static_assert( operator_less_equal_002( ) );

	constexpr bool operator_greater_001( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );

		return l > f;
	}
	static_assert( operator_greater_001( ) );

	constexpr bool operator_greater_equal_001( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );
		auto l = daw::iend( numbers );

		return l >= f;
	}
	static_assert( operator_greater_equal_001( ) );

	constexpr bool operator_greater_equal_002( ) {
		std::array<int, 7> const numbers = { 1, 2, 3, 4, 5, 6, 7 };
		auto f = daw::ibegin( numbers );

		return f >= f;
	}
	static_assert( operator_greater_equal_002( ) );
} // namespace

int main( ) {}
