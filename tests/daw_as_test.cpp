// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_as.h>

#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace {
	enum unscoped_enum { enum_value = 17 };

	struct base {
		virtual ~base( ) = default;
	};

	struct derived : base {};

	struct implicitly_convertible {
		int value;

		constexpr operator int( ) const noexcept {
			return value;
		}
	};

	struct explicitly_convertible {
		int value;

		explicit constexpr operator int( ) const noexcept {
			return value;
		}
	};

	template<typename To, typename From, typename = void>
	struct can_as : std::false_type {};

	template<typename To, typename From>
	struct can_as<To, From,
	              std::void_t<decltype( daw::as<To>(
	                std::declval<From>( ) ) )>> : std::true_type {};

	template<typename To, typename From>
	inline constexpr bool can_as_v = can_as<To, From>::value;

	static_assert( std::is_same_v<decltype( daw::as<long>( 1 ) ), long> );
	static_assert( std::is_same_v<decltype( daw::as<std::string_view>(
	                                std::declval<std::string const &>( ) ) ),
	                              std::string_view> );

	static_assert( daw::as<int>( 3.75 ) == 3 );
	static_assert( daw::as<int>( enum_value ) == 17 );
	static_assert( daw::as<int>( implicitly_convertible{ 42 } ) == 42 );
	static_assert( daw::as<int>( explicitly_convertible{ 43 } ) == 43 );

	static_assert( can_as_v<long, int> );
	static_assert( can_as_v<int, explicitly_convertible> );
	static_assert( can_as_v<std::string_view, std::string const &> );
	static_assert( not can_as_v<int *, std::string> );
	static_assert( not can_as_v<int, std::string> );
} // namespace

int main( ) {
	auto const text = std::string{ "hello" };
	auto const view = daw::as<std::string_view>( text );
	assert( view == "hello" );
	assert( view.data( ) == text.data( ) );

	derived object;
	derived *derived_ptr = &object;
	base *base_ptr = daw::as<base *>( derived_ptr );
	assert( base_ptr == &object );
}
