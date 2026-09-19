// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

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
		static_assert( daw::explicitly_convertible_to<iterator, const_iterator> );

	private:
		iterator m_first{ };

	public:
		skip_view( ) = default;

		explicit constexpr skip_view( daw::constructible<base_t> auto &&r,
		                              std::size_t how_many )
		  : base_t( DAW_FWD( r ) )
		  , m_first( base_t::begin( ) ) {
			if constexpr( RandomIterator<iterator> ) {
				auto const range_size =
				  pimpl::ranges_distance( m_first, base_t::end( ) );
				auto const skip =
				  std::min( { static_cast<std::ptrdiff_t>( how_many ), range_size } );
				m_first = std::next( m_first, skip );
			} else {
				// Input
				auto const last = base_t::end( );
				for( auto n = how_many; n > 0; --n ) {
					if( m_first == last ) {
						break;
					}
					++m_first;
				}
			}
		}

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
