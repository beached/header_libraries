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
		static_assert( daw::explicitly_convertible_to<iterator, const_iterator> );

	private:
		iterator m_first{ };

		[[nodiscard]] static constexpr iterator
		drop_while( iterator first, iterator_last last, auto &&fn ) {
			while( first != last ) {
				if( not std::invoke( fn, as_const( *first ) ) ) {
					break;
				}
				++first;
			}
			return first;
		}

		[[nodiscard]] static constexpr iterator
		drop_until( iterator first, iterator_last last, auto &&fn ) {
			while( first != last ) {
				if( std::invoke( fn, as_const( *first ) ) ) {
					break;
				}
				++first;
			}
			return first;
		}

	public:
		drop_view( ) = default;

		explicit constexpr drop_view(
		  daw::constructible<base_t> auto &&r, drop_while_t,
		  InvocableAs<bool( range_const_reference_t<R> )> auto &&fn )
		  : base_t( DAW_FWD( r ) )
		  , m_first{
		      drop_while( base_t::begin( ), base_t::end( ), DAW_FWD( fn ) ) } {}

		explicit constexpr drop_view(
		  daw::constructible<base_t> auto &&r, drop_until_t,
		  InvocableAs<bool( range_const_reference_t<R> )> auto &&fn )
		  : base_t( DAW_FWD( r ) )
		  , m_first{
		      drop_until( base_t::begin( ), base_t::end( ), DAW_FWD( fn ) ) } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return m_first;
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return static_cast<const_iterator>( m_first );
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