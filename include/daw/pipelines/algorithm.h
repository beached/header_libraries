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

	namespace pimpl {
		template<typename Compare>
		struct Sort_t {
			DAW_NO_UNIQUE_ADDRESS Compare m_compare{ };

			template<typename C>
			requires( not Range<C> ) //
			  [[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( C &&c ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return Sort_t<C>( DAW_FWD( c ) );
			}

			[[nodiscard]] constexpr decltype( auto )
			operator( )( Sortable auto &&r ) const {
				std::sort( std::begin( r ), std::end( r ), m_compare );
				return DAW_FWD( r );
			}
		};
		Sort_t( ) -> Sort_t<std::less<>>;
		template<typename Compare>
		Sort_t( Compare ) -> Sort_t<Compare>;

		template<typename Compare>
		struct Max_t : Compare {
			template<typename C>
			requires( not Range<C> ) //
			  [[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( C &&c ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return Max_t<C>( DAW_FWD( c ) );
			}

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
			template<typename C>
			requires( not Range<C> ) //
			  [[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( C &&c ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return Min_t<C>( DAW_FWD( c ) );
			}

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
			template<typename C>
			requires( not Range<C> ) //
			  [[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( C &&c ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return MinMax_t<C>( DAW_FWD( c ) );
			}

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
	} // namespace pimpl

	inline constexpr auto Sort = pimpl::Sort_t{ };
	inline constexpr auto Max = pimpl::Max_t{ };
	inline constexpr auto Min = pimpl::Min_t{ };
	inline constexpr auto MinMax = pimpl::MinMax_t{ };
} // namespace daw::pipelines
