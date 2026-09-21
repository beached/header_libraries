// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_as.h"
#include "daw/daw_iterator_traits.h"
#include "daw/pipelines/maybe_owning_range.h"
#include "daw/pipelines/sized_iterator.h"
#include "daw/pipelines/view.h"

#include <algorithm>
#include <cstddef>
#include <iterator>

namespace daw::pipelines {
	template<typename R>
	struct skip_view : private maybe_owning_range<R> {
		using base_t = maybe_owning_range<R>;
		using iterator = typename base_t::iterator;
		using const_iterator = typename base_t::const_iterator;
		using iterator_last = typename base_t::iterator_last;
		using const_iterator_last = typename base_t::const_iterator_last;
		using daw_i_am_a_skip_view_class = void;

	private:
		using difference_type = daw::range_difference_t<R>;
		// How many leading elements are skipped.  An offset, unlike a cached
		// iterator, stays valid when the view is copied or moved.
		difference_type m_skipped = 0;

	public:
		skip_view( ) = default;

		explicit constexpr skip_view( daw::constructible<base_t> auto &&r,
		                              std::size_t how_many )
		  : base_t( DAW_FWD( r ) ) {
			auto first = base_t::begin( );
			auto const last = base_t::end( );
			if constexpr( RandomIterator<iterator> ) {
				auto const range_size = pimpl::ranges_distance( first, last );
				m_skipped = std::min( { as<difference_type>( how_many ), range_size } );
			} else {
				// Input
				for( ; how_many > 0 and first != last; --how_many ) {
					++first;
					++m_skipped;
				}
			}
		}

		[[nodiscard]] constexpr iterator begin( ) {
			return std::next( base_t::begin( ), m_skipped );
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return std::next( base_t::begin( ), m_skipped );
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return base_t::end( );
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return base_t::end( );
		}

		[[nodiscard]] constexpr bool
		operator==( skip_view const & ) const = default;
	};
	template<typename R>
	skip_view( R && ) -> skip_view<R>;
} // namespace daw::pipelines

namespace daw::pipelines::pimpl {
	struct Skip_t {
		std::size_t how_many;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			return skip_view<R>{ DAW_FWD( r ), how_many };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] constexpr auto Skip( std::size_t how_many ) {
		return pimpl::Skip_t{ how_many };
	}
} // namespace daw::pipelines
