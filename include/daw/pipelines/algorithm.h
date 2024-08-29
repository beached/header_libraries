// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "range.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>

namespace daw::pipelines {
	template<typename R>
	concept Sortable = Range<R> and RandomIterator<iterator_t<R>>;

	namespace pimpl {
		template<typename Compare = std::less<>,
		         typename Projection = std::identity>
		struct Sort_t {
			DAW_NO_UNIQUE_ADDRESS Compare m_compare{ };
			DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };

			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( auto &&compare ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return Sort_t{ DAW_FWD( compare ) };
			}

			[[nodiscard]] constexpr decltype( auto )
			operator( )( Sortable auto &&r ) const {
				std::sort( std::begin( r ),
				           std::end( r ),
				           [&]( auto const &lhs, auto const &rhs ) {
					           return std::invoke( m_compare,
					                               std::invoke( m_projection, lhs ),
					                               std::invoke( m_projection, rhs ) );
				           } );
				return DAW_FWD( r );
			}
		};
		Sort_t( ) -> Sort_t<std::less<>, std::identity>;

		template<typename Compare>
		Sort_t( Compare ) -> Sort_t<Compare, std::identity>;

		template<typename Compare, typename Projection>
		Sort_t( Compare, Projection ) -> Sort_t<Compare, Projection>;

		template<typename Compare = std::less<>,
		         typename Projection = std::identity>
		struct Max_t {
			DAW_NO_UNIQUE_ADDRESS Compare m_compare{ };
			DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };

			template<typename C, typename P = std::identity>
			requires( not Range<C> ) //
			  [[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( C &&compare, P &&projection = Projection{ } )
			    DAW_CPP23_STATIC_CALL_OP_CONST {
				return Max_t{ DAW_FWD( compare ), DAW_FWD( projection ) };
			}

			[[nodiscard]] constexpr auto operator( )( Range auto &&r ) const {
				return std::max_element( std::begin( r ),
				                         std::end( r ),
				                         [&]( auto const &lhs, auto const &rhs ) {
					                         return std::invoke(
					                           m_compare,
					                           std::invoke( m_projection, lhs ),
					                           std::invoke( m_projection, rhs ) );
				                         } );
			}
		};
		Max_t( ) -> Max_t<>;

		template<typename Compare>
		Max_t( Compare ) -> Max_t<Compare>;

		template<typename Compare, typename Projection>
		Max_t( Compare, Projection ) -> Max_t<Compare, Projection>;

		template<typename Compare = std::less<>,
		         typename Projection = std::identity>
		struct Min_t {
			DAW_NO_UNIQUE_ADDRESS Compare m_compare{ };
			DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };

			template<typename C, typename P = std::identity>
			requires( not Range<C> ) //
			  [[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( C &&compare, P &&projection = Projection{ } )
			    DAW_CPP23_STATIC_CALL_OP_CONST {
				return Min_t{ DAW_FWD( compare ), DAW_FWD( projection ) };
			}

			[[nodiscard]] constexpr auto operator( )( Range auto &&r ) const {
				return std::min_element( std::begin( r ),
				                         std::end( r ),
				                         [&]( auto const &lhs, auto const &rhs ) {
					                         return std::invoke(
					                           m_compare,
					                           std::invoke( m_projection, lhs ),
					                           std::invoke( m_projection, rhs ) );
				                         } );
			}
		};
		Min_t( ) -> Min_t<>;

		template<typename Compare>
		Min_t( Compare ) -> Min_t<Compare>;

		template<typename Compare, typename Projection>
		Min_t( Compare, Projection ) -> Min_t<Compare, Projection>;

		template<typename Compare = std::less<>,
		         typename Projection = std::identity>
		struct MinMax_t {
			DAW_NO_UNIQUE_ADDRESS Compare m_compare{ };
			DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };

			template<typename C, typename P = std::identity>
			requires( not Range<C> ) //
			  [[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			  operator( )( C &&compare, P &&projection = Projection{ } )
			    DAW_CPP23_STATIC_CALL_OP_CONST {
				return MinMax_t{ DAW_FWD( compare ), DAW_FWD( projection ) };
			}

			[[nodiscard]] constexpr auto operator( )( Range auto &&r ) const {
				auto result = std::minmax_element(
				  std::begin( r ),
				  std::end( r ),
				  [&]( auto const &lhs, auto const &rhs ) {
					  return std::invoke( m_compare,
					                      std::invoke( m_projection, lhs ),
					                      std::invoke( m_projection, rhs ) );
				  } );
				return std::tuple{ result.first, result.second };
			}
		};
		MinMax_t( ) -> MinMax_t<>;

		template<typename Compare>
		MinMax_t( Compare ) -> MinMax_t<Compare>;

		template<typename Compare, typename Projection>
		MinMax_t( Compare, Projection ) -> MinMax_t<Compare, Projection>;
	} // namespace pimpl

	inline constexpr auto Sort = pimpl::Sort_t{ };
	inline constexpr auto Max = pimpl::Max_t{ };
	inline constexpr auto Min = pimpl::Min_t{ };
	inline constexpr auto MinMax = pimpl::MinMax_t{ };
} // namespace daw::pipelines
