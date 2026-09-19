// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_move.h"
#include "daw/pipelines/maybe_owning_range.h"
#include "daw/pipelines/view.h"

namespace daw::pipelines {
	inline constexpr struct drop_while_t {
		explicit drop_while_t( ) = default;
	} drop_while{ };

	inline constexpr struct drop_until_t {
		explicit drop_until_t( ) = default;
	} drop_until{ };

	template<Range R>
	struct drop_view : private maybe_owning_range<R> {
		using base_t = maybe_owning_range<R>;
		using iterator = typename base_t::iterator;
		using const_iterator = typename base_t::const_iterator;
		using iterator_last = typename base_t::iterator_last;
		using const_iterator_last = typename base_t::const_iterator_last;
		using daw_i_am_a_drop_view_class = void;

	private:
		using difference_type = std::ptrdiff_t;
		// How many leading elements are dropped.  An offset, unlike a cached
		// iterator, stays valid when the view is copied or moved.
		difference_type m_dropped = 0;

		[[nodiscard]] static constexpr difference_type
		drop_while( iterator first, iterator_last last, auto &&fn ) {
			difference_type n = 0;
			while( first != last ) {
				if( not std::invoke( fn, as_const( *first ) ) ) {
					break;
				}
				++first;
				++n;
			}
			return n;
		}

		[[nodiscard]] static constexpr difference_type
		drop_until( iterator first, iterator_last last, auto &&fn ) {
			difference_type n = 0;
			while( first != last ) {
				if( std::invoke( fn, as_const( *first ) ) ) {
					break;
				}
				++first;
				++n;
			}
			return n;
		}

	public:
		drop_view( ) = default;

		explicit constexpr drop_view(
		  daw::constructible<base_t> auto &&r, drop_while_t,
		  InvocableAs<bool( range_const_reference_t<R> )> auto &&fn )
		  : base_t( DAW_FWD( r ) )
		  , m_dropped{
		      drop_while( base_t::begin( ), base_t::end( ), DAW_FWD( fn ) ) } {}

		explicit constexpr drop_view(
		  daw::constructible<base_t> auto &&r, drop_until_t,
		  InvocableAs<bool( range_const_reference_t<R> )> auto &&fn )
		  : base_t( DAW_FWD( r ) )
		  , m_dropped{
		      drop_until( base_t::begin( ), base_t::end( ), DAW_FWD( fn ) ) } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return std::next( base_t::begin( ), m_dropped );
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return std::next( base_t::begin( ), m_dropped );
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return base_t::end( );
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return base_t::end( );
		}

		[[nodiscard]] constexpr bool
		operator==( drop_view const & ) const = default;
	};

	template<typename Pred>
	constexpr auto DropWhile( Pred &&p ) {
		return [pred = DAW_FWD( p )]<Range R>( R &&r ) {
			return drop_view<R>{ DAW_FWD( r ), drop_while, pred };
		};
	}

	template<typename Pred>
	constexpr auto DropUntil( Pred &&p ) {
		return [pred = DAW_FWD( p )]<Range R>( R &&r ) {
			return drop_view<R>{ DAW_FWD( r ), drop_until, pred };
		};
	}
} // namespace daw::pipelines