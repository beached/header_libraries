// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../daw_move.h"

#include <ciso646>
#include <cstddef>
#include <type_traits>

namespace daw {
	namespace math_impl {
		struct less {
			template<typename T, typename U>
			constexpr auto operator( )( T const &lhs, U const &rhs ) const {
				return lhs < rhs;
			}
		};
	} // namespace math_impl
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

			if( math_impl::less{ }( tmp, val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( tmp );
		} else if constexpr( sizeof...( Ts ) == 1 ) {
			using result_t = std::common_type_t<T, Ts...>;
			std::common_type_t<Ts...> const rhs[1]{ vs... };
			if( math_impl::less{ }( rhs[0], val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( rhs[0] );
		} else {
			return val1;
		}
	}

	template<typename T, typename Compare = math_impl::less>
	constexpr decltype( auto ) min_comp( T &&lhs, T &&rhs,
	                                     Compare &&comp = Compare{ } ) {
		if( comp( lhs, rhs ) ) {
			return DAW_FWD2( T, lhs );
		}
		return DAW_FWD2( T, rhs );
	}

	template<typename T, typename Compare = math_impl::less>
	constexpr decltype( auto ) max_comp( T &&lhs, T &&rhs,
	                                     Compare &&comp = Compare{ } ) {
		if( comp( rhs, lhs ) ) {
			return DAW_FWD2( T, lhs );
		}
		return DAW_FWD2( T, rhs );
	}

	template<typename T, typename... Ts>
	constexpr auto( min )( T const &val1, Ts const &...vs ) noexcept {
		if constexpr( sizeof...( Ts ) > 1 ) {
			auto const tmp = (min)( vs... );
			using result_t = std::common_type_t<T, decltype( tmp )>;

			if( not math_impl::less{ }( tmp, val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( tmp );
		} else if constexpr( sizeof...( Ts ) == 1 ) {
			using result_t = std::common_type_t<T, Ts...>;
			std::common_type_t<Ts...> const rhs[1]{ vs... };
			if( not math_impl::less{ }( rhs[0], val1 ) ) {
				return static_cast<result_t>( val1 );
			}
			return static_cast<result_t>( rhs[0] );
		} else {
			return val1;
		}
	}
} // namespace daw
