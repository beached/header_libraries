// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_as.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_typeof.h"
#include "daw/pipelines/counted_source.h"
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
	/// Holds the source range and the requested count only.  The sized
	/// iterators are built in begin( ), so they always refer to this object's
	/// current location and the view can be safely copied and moved, like
	/// map_view and filter_view.  Caching them at construction would leave
	/// their parent pointers dangling after a copy/move.
	template<Range R>
	struct take_view : private pimpl::stored_range_base_t<R> {
		using base_t = pimpl::stored_range_base_t<R>;

	private:
		template<typename I, typename L>
		using sized_t = std::conditional_t<RandomIterator<I>, sized_iterator<I>,
		                                   sized_iterator<I, L>>;

		std::size_t m_how_many = 0;

		template<typename It, typename F, typename L>
		[[nodiscard]] constexpr It make_begin( F first, L last ) const {
			if constexpr( RandomIterator<F> ) {
				auto const range_size = [&] {
					if constexpr( pimpl::counted_source<F, L> ) {
						return pimpl::counted_length( first, last );
					} else {
						return pimpl::ranges_distance( first, last );
					}
				}( );
				auto const take_size =
				  std::min( { range_size, as<std::ptrdiff_t>( m_how_many ) } );
				return It{ std::move( first ), as<std::size_t>( take_size ) };
			} else {
				return It{ std::move( first ), std::move( last ), m_how_many };
			}
		}

	public:
		using iterator = sized_t<iterator_t<R>, iterator_end_t<R>>;
		using const_iterator =
		  sized_t<const_iterator_t<R>, const_iterator_end_t<R>>;
		using iterator_last = sized_iterator_end<iterator_t<R>>;
		using const_iterator_last = sized_iterator_end<const_iterator_t<R>>;

		explicit take_view( ) = default;

		template<Range R0>
		requires( std::constructible_from<base_t, R0> ) //
		  explicit constexpr take_view( R0 &&r, std::size_t how_many )
		  : base_t( DAW_FWD( r ) )
		  , m_how_many( how_many ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return make_begin<iterator>( base_t::rbegin( ), base_t::rend( ) );
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return make_begin<const_iterator>( base_t::rbegin( ), base_t::rend( ) );
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return iterator_last{ };
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return const_iterator_last{ };
		}
	};

	template<ForwardRange R, typename Predicate>
	struct take_while_view : private pimpl::stored_range_base_t<R> {
		using base_t = pimpl::stored_range_base_t<R>;

	private:
		DAW_NO_UNIQUE_ADDRESS Predicate m_predicate{ };

		template<typename It, typename F, typename L>
		[[nodiscard]] static constexpr It make_last( F first, L last,
		                                             Predicate pred ) {
			while( first != last and std::invoke( pred, *first ) ) {
				++first;
			}
			return first;
		}

	public:
		using iterator = daw::iterator_t<R>;
		using const_iterator = daw::const_iterator_t<R>;
		using iterator_last = iterator;
		using const_iterator_last = const_iterator;

		explicit take_while_view( ) = default;

		explicit constexpr take_while_view(
		  daw::constructible<base_t> auto &&r,
		  daw::constructible<Predicate> auto &&pred )
		  : base_t( DAW_FWD( r ) )
		  , m_predicate( DAW_FWD( pred ) ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return base_t::rbegin( );
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return base_t::rbegin( );
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return make_last<iterator_last>(
			  base_t::rbegin( ), base_t::rend( ), m_predicate );
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return make_last<const_iterator_last>(
			  base_t::rbegin( ), base_t::rend( ), m_predicate );
		}
	};
	template<typename R, typename F>
	take_while_view( R &&, F ) -> take_while_view<R, F>;
} // namespace daw::pipelines

namespace daw::pipelines::pimpl {
	struct Take_t {
		std::size_t how_many = 0;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			return take_view<R>{ DAW_FWD( r ), how_many };
		}
	};

	template<typename Fn>
	struct TakeWhile_t {
		DAW_NO_UNIQUE_ADDRESS Fn m_func;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			return take_while_view{ DAW_FWD( r ), m_func };
		}
	};
	template<typename Fn>
	TakeWhile_t( Fn ) -> TakeWhile_t<Fn>;

	template<typename Fn>
	struct TakeUntil_t {
		DAW_NO_UNIQUE_ADDRESS struct MakeWhileFn_t {
			DAW_NO_UNIQUE_ADDRESS Fn m_fn;

			[[nodiscard]] constexpr decltype( auto ) operator( )( auto &&value ) {
				return not std::invoke( m_fn, DAW_FWD( value ) );
			}

			[[nodiscard]] constexpr decltype( auto )
			operator( )( auto &&value ) const {
				return not std::invoke( m_fn, DAW_FWD( value ) );
			}
		} m_pred;

		explicit constexpr TakeUntil_t( daw::constructible<Fn> auto &&pred )
		  : m_pred{ DAW_FWD( pred ) } {}

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			return take_while_view{ DAW_FWD( r ), m_pred };
		}
	};
	template<typename Fn>
	TakeUntil_t( Fn ) -> TakeUntil_t<Fn>;
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] constexpr auto TakeWhile( auto &&fn ) {
		return pimpl::TakeWhile_t{ DAW_FWD( fn ) };
	}

	[[nodiscard]] constexpr auto TakeUntil( auto &&fn ) {
		return pimpl::TakeUntil_t{ DAW_FWD( fn ) };
	}

	[[nodiscard]] constexpr auto Take( std::size_t how_many ) {
		return pimpl::Take_t{ how_many };
	}
} // namespace daw::pipelines
