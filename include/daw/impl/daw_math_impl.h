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

namespace daw {
	template<typename Float, std::enable_if_t<std::is_floating_point_v<Float>,
	                                          std::nullptr_t> = nullptr>
	constexpr Float float_abs( Float f ) {
		if( f < 0 ) {
			return -f;
		}
		return f;
	}

	template<typename T, typename... Ts>
	constexpr auto( max )( T const &val1, Ts const &...vs ) noexcept {
		if constexpr( sizeof...( Ts ) > 1 ) {
			auto const tmp = (max)( vs... );
			using result_t = std::common_type_t<T, decltype( tmp )>;

			if( std::less<>{ }( tmp, val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( tmp );
		} else if constexpr( sizeof...( Ts ) == 1 ) {
			using result_t = std::common_type_t<T, Ts...>;
			std::common_type_t<Ts...> const rhs[1]{ vs... };
			if( std::less<>{ }( rhs[0], val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( rhs[0] );
		} else {
			return val1;
		}
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

	template<typename T, typename... Ts>
	constexpr auto( min )( T const &val1, Ts const &...vs ) noexcept {
		if constexpr( sizeof...( Ts ) > 1 ) {
			auto const tmp = (min)( vs... );
			using result_t = std::common_type_t<T, decltype( tmp )>;

			if( not std::less<>{ }( tmp, val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( tmp );
		} else if constexpr( sizeof...( Ts ) == 1 ) {
			using result_t = std::common_type_t<T, Ts...>;
			std::common_type_t<Ts...> const rhs[1]{ vs... };
			if( not std::less<>{ }( rhs[0], val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( rhs[0] );
		} else {
			return val1;
		}
	}
} // namespace daw
