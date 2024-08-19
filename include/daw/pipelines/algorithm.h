// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "range.h"

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace daw::pipelines {
	template<typename R>
	concept Sortable = Range<R> and RandomIterator<iterator_t<R>>;

	namespace impl {
		template<typename Compare>
		struct Sort_t : Compare {
			[[nodiscard]] constexpr decltype( auto )
			operator( )( Sortable auto &&r ) const {
				std::sort( std::begin( r ),
				           std::end( r ),
				           *static_cast<Compare const *>( this ) );
				return DAW_FWD( r );
			}
		};
		Sort_t( ) -> Sort_t<std::less<>>;
		template<typename Compare>
		Sort_t( Compare ) -> Sort_t<Compare>;

		template<typename Compare>
		struct Max_t : Compare {
			[[nodiscard]] constexpr auto operator( )( Range auto &&r ) const {
				return std::max_element( std::begin( r ),
				                         std::end( r ),
				                         *static_cast<Compare const *>( this ) );
			}
		};
		Max_t( ) -> Max_t<std::less<>>;
		template<typename Compare>
		Max_t( Compare ) -> Max_t<Compare>;

		template<typename Compare>
		struct Min_t : Compare {
			[[nodiscard]] constexpr auto operator( )( Range auto &&r ) const {
				return std::min_element( std::begin( r ),
				                         std::end( r ),
				                         *static_cast<Compare const *>( this ) );
			}
		};
		Min_t( ) -> Min_t<std::less<>>;
		template<typename Compare>
		Min_t( Compare ) -> Min_t<Compare>;

		template<typename Compare>
		struct MinMax_t : Compare {
			[[nodiscard]] constexpr auto operator( )( Range auto &&r ) const {
				auto result =
				  std::minmax_element( std::begin( r ),
				                       std::end( r ),
				                       *static_cast<Compare const *>( this ) );
				return std::tuple{ result.first, result.second };
			}
		};
		MinMax_t( ) -> MinMax_t<std::less<>>;
		template<typename Compare>
		MinMax_t( Compare ) -> MinMax_t<Compare>;
	} // namespace impl

	[[nodiscard]] constexpr auto Sort( ) {
		return impl::Sort_t{ };
	}

	[[nodiscard]] constexpr auto SortC( auto &&compare ) {
		return impl::Sort_t{ DAW_FWD( compare ) };
	}

	[[nodiscard]] constexpr auto Max( ) {
		return impl::Max_t{ };
	}

	[[nodiscard]] constexpr auto MaxC( auto &&compare ) {
		return impl::Max_t{ DAW_FWD( compare ) };
	}

	[[nodiscard]] constexpr auto Min( ) {
		return impl::Min_t{ };
	}

	[[nodiscard]] constexpr auto MinC( auto &&compare ) {
		return impl::Min_t{ DAW_FWD( compare ) };
	}

	[[nodiscard]] constexpr auto MinMax( ) {
		return impl::MinMax_t{ };
	}

	[[nodiscard]] constexpr auto MinMaxC( auto &&compare ) {
		return impl::MinMax_t{ DAW_FWD( compare ) };
	}
} // namespace daw::pipelines
