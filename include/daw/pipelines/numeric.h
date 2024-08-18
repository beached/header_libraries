// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"

#include <cstddef>
#include <iterator>

namespace daw::pipelines {
	namespace impl {
		struct sum_t {
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( Range auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				using value_type = daw::iter_value_t<decltype( std::begin( r ) )>;
				auto sum = value_type{ };
				for( auto const &v : r ) {
					sum = sum + v;
				}
				return sum;
			}
		};

		struct count_t {
			template<typename R>
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr std::size_t
			operator( )( R const &r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				if constexpr( not Range<R> ) {
					return 1;
				} else if constexpr( requires { r.size( ); } ) {
					return r.size( );
				} else {
					return static_cast<std::size_t>(
					  std::distance( std::begin( r ), std::end( r ) ) );
				}
			}
		};
	} // namespace impl

	inline constexpr auto Sum = impl::sum_t{ };
	inline constexpr auto Count = impl::count_t{ };

	[[nodiscard]] constexpr auto CountIf( auto &&fn ) {
		return [func = DAW_FWD( fn )]( Range auto const &r ) {
			std::size_t result = 0;
			for( auto const &v : r ) {
				result += static_cast<std::size_t>( func( v ) );
			}
			return result;
		};
	};
} // namespace daw::pipelines
