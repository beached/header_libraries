// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <ciso646>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#ifdef max
#undef max
#endif // max
#ifdef min
#undef min
#endif // min

namespace daw {
	template<typename Float, std::enable_if_t<std::is_floating_point_v<Float>,
	                                          std::nullptr_t> = nullptr>
	constexpr Float float_abs( Float f ) {
		if( f < 0 ) {
			return -f;
		}
		return f;
	}

	template<typename T>
	constexpr T max( T &&t ) noexcept {
		return std::forward<T>( t );
	}

	template<typename T0, typename T1>
	constexpr auto max( T0 &&val1, T1 &&val2 ) noexcept
	  -> std::common_type_t<T0, T1> {
		if( val1 > val2 ) {
			return std::forward<T0>( val1 );
		}
		return std::forward<T1>( val2 );
	}

	template<typename T0, typename T1, typename... Ts,
	         std::enable_if_t<( sizeof...( Ts ) != 0 ), std::nullptr_t> = nullptr>
	constexpr auto max( T0 &&val1, T1 &&val2, Ts &&...vs ) noexcept
	  -> std::common_type_t<T0, T1, Ts...> {
		auto tmp = max( std::forward<Ts>( vs )... );
		if( val1 > val2 ) {
			if( val1 > tmp ) {
				return std::forward<T0>( val1 );
			}
			return tmp;
		} else if( val2 > tmp ) {
			return std::forward<T1>( val2 );
		}
		return tmp;
	}

	template<typename T>
	constexpr decltype( auto ) min( T &&t ) noexcept {
		return std::forward<T>( t );
	}

	template<typename T0, typename T1>
	constexpr auto min( T0 &&val1, T1 &&val2 ) noexcept
	  -> std::common_type_t<T0, T1> {

		if( std::less_equal<>{ }( val1, val2 ) ) {
			return std::forward<T0>( val1 );
		}
		return std::forward<T1>( val2 );
	}

	template<typename T, typename Compare = std::less<>>
	constexpr decltype( auto ) min_comp( T &&lhs, T &&rhs,
	                                     Compare &&comp = Compare{ } ) {
		if( comp( lhs, rhs ) ) {
			return std::forward<T>( lhs );
		}
		return std::forward<T>( rhs );
	}

	template<typename T, typename Compare = std::less<>>
	constexpr decltype( auto ) max_comp( T &&lhs, T &&rhs,
	                                     Compare &&comp = Compare{ } ) {
		if( comp( rhs, lhs ) ) {
			return std::forward<T>( lhs );
		}
		return std::forward<T>( rhs );
	}

	template<typename T, typename... Ts,
	         std::enable_if_t<( sizeof...( Ts ) != 0 ), std::nullptr_t> = nullptr>
	constexpr auto min( T &&val1, Ts &&...vs ) noexcept
	  -> std::common_type_t<T, Ts...> {

		auto &&tmp = min( std::forward<Ts>( vs )... );

		if( std::less_equal<>{ }( val1, tmp ) ) {
			return std::forward<T>( val1 );
		}
		return std::forward<decltype( tmp )>( tmp );
	}

} // namespace daw
