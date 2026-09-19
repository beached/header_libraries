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
#include "daw/daw_typeof.h"
#include "daw/pipelines/filter.h"
#include "daw/pipelines/maybe_owning_range.h"
#include "daw/pipelines/sized_iterator.h"
#include "daw/pipelines/skip.h"
#include "daw/pipelines/view.h"
#include "daw_concept_checker.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>

namespace daw::pipelines {
	template<Range R, typename First, typename Last>
	struct take_view {
		using iterator = First;
		using iterator_last = Last;

	private:
		using storage_t = maybe_owning_range<R>;
		storage_t m_storage{ };

		struct iters_t {
			iterator first{ };
			iterator_last last{ };

			iters_t( ) = default;
			constexpr iters_t( std::pair<iterator, iterator_last> p )
			  : first{ std::move( p.first ) }
			  , last{ std::move( p.second ) } {}
		} m_iters{ };

	public:
		explicit take_view( ) = default;

		template<daw::constructible<storage_t> R0>
		explicit take_view(
		  R0 &&r,
		  InvocableAs<std::pair<iterator, iterator_last>( R & )> auto &&func )
		  : m_storage( DAW_FWD( r ) )
		  , m_iters{ func( m_storage.get_range( ) ) } {}

		[[nodiscard]] constexpr iterator begin( ) const {
			return m_iters.first;
		}

		[[nodiscard]] constexpr iterator_last end( ) const {
			return m_iters.last;
		}

		[[nodiscard]] constexpr bool
		operator==( take_view const & ) const = default;
	};
} // namespace daw::pipelines

namespace daw::pipelines::pimpl {
	struct Take_t {
		std::size_t how_many = 0;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			using iterator = iterator_t<decltype( r )>;
			static_assert( check_input_iterator<iterator>( ) );
			using iterator_last = iterator_end_t<decltype( r )>;
			static_assert( check_input_iterator<iterator_last>( ) );
			iterator first = std::begin( r );
			iterator_last last = std::end( r );

			if constexpr( RandomIterator<iterator> ) {
				auto const range_size = pimpl::ranges_distance( first, last );
				auto const take_size =
				  std::min( { range_size, static_cast<std::ptrdiff_t>( how_many ) } );
				using first_t = sized_iterator<iterator>;
				using last_t = sized_iterator_end<iterator>;
				using take_t = take_view<R, first_t, last_t>;
				static_assert( check_random_access_range<take_t>( ) );
				return take_t{ DAW_FWD( r ), [take_size]( Range auto &&r0 ) {
					              return std::pair{
					                first_t{ std::begin( r0 ),
					                         static_cast<std::size_t>( take_size ) },
					                last_t{ } };
				              } };
			} else {
				using first_t = sized_iterator<iterator, iterator_last>;
				using last_t = sized_iterator_end<iterator>;
				using take_t = take_view<R, first_t, last_t>;
				static_assert( check_input_range<take_t>( ) );
				return take_t{ DAW_FWD( r ), [count = how_many]( Range auto &&r0 ) {
					              return std::pair{
					                first_t{ std::begin( r0 ), std::end( r0 ), count },
					                last_t{ } };
				              } };
			}
		}
	};

	template<typename Fn>
	struct TakeWhile_t {
		DAW_NO_UNIQUE_ADDRESS mutable Fn m_func;
		mutable bool m_trigger = false;

		[[nodiscard]] constexpr auto operator( )( auto &&value ) const {
			if( m_trigger ) {
				return true;
			}
			if( not m_func( DAW_FWD( value ) ) ) {
				m_trigger = true;
				return true;
			}
			return false;
		}
	};
	template<typename Fn>
	TakeWhile_t( Fn ) -> TakeWhile_t<Fn>;

	template<typename Fn>
	struct TakeUntil_t {
		DAW_NO_UNIQUE_ADDRESS mutable Fn m_func;
		mutable bool m_trigger = false;

		[[nodiscard]] constexpr auto operator( )( auto &&value ) const {
			if( m_trigger ) {
				return false;
			}
			if( m_func( DAW_FWD( value ) ) ) {
				m_trigger = true;
				return false;
			}
			return true;
		}
	};
	template<typename Fn>
	TakeUntil_t( Fn ) -> TakeUntil_t<Fn>;
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] constexpr auto TakeWhile( auto &&fn ) {
		return Filter( pimpl::TakeWhile_t{ DAW_FWD( fn ) } );
	}

	[[nodiscard]] constexpr auto TakeUntil( auto &&fn ) {
		return Filter( pimpl::TakeUntil_t{ DAW_FWD( fn ) } );
	}

	[[nodiscard]] constexpr auto Take( std::size_t how_many ) {
		return pimpl::Take_t{ how_many };
	}
} // namespace daw::pipelines
