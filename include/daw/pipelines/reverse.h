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
#include "daw/iterator/daw_reverse_iterator.h"
#include "daw/pipelines/maybe_owning_range.h"
#include "daw/pipelines/move_next.h"
#include "daw/pipelines/view.h"

#include <utility>

namespace daw::pipelines {
	template<BidirectionalRange R>
	struct reverse_view : private maybe_owning_range<R> {
		using base_t = maybe_owning_range<R>;
		using iterator = daw::reverse_iterator<typename base_t::iterator>;
		using const_iterator =
		  daw::reverse_iterator<typename base_t::const_iterator>;
		using iterator_last = daw::reverse_iterator<typename base_t::iterator_last>;
		using const_iterator_last =
		  daw::reverse_iterator<typename base_t::const_iterator_last>;
		using daw_i_am_a_reverse_view_class = void;

		using base_t::is_owned;

		[[nodiscard]] constexpr auto &get_range( ) &
		  requires( not std::is_const_v<std::remove_reference_t<R>> ) {
			return static_cast<base_t *>( this )->get_range( );
		}

		[[nodiscard]] constexpr auto const &get_range( ) const & {
			return static_cast<base_t const *>( this )->get_range( );
		}

		reverse_view( ) = default;

		explicit constexpr reverse_view( daw::constructible<base_t> auto &&r )
		  : base_t{ DAW_FWD( r ) } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ base_t::end( ) };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{ base_t::end( ) };
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return iterator_last{ base_t::begin( ) };
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return const_iterator_last{ base_t::begin( ) };
		}

		[[nodiscard]] constexpr bool
		operator==( reverse_view const & ) const = default;
	};
	template<typename R>
	reverse_view( R && ) -> reverse_view<R>;
} // namespace daw::pipelines

namespace daw::pipelines::pimpl {
	struct Reverse_t {
		explicit Reverse_t( ) = default;

		template<BidirectionalRange R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr daw::remove_rvalue_ref_t<R>
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			auto first = std::begin( r );
			auto const last = std::end( r );
			auto it = safe_move_next( first, last );
			auto tail = it;

			if constexpr( RandomRange<R> ) {
				if( first != last ) {
					--tail;
					while( first < tail ) {
						std::iter_swap( first, tail );
						++first;
						--tail;
					}
				}
			} else {
				while( true ) {
					if( first == tail || first == --tail ) {
						break;
					}
					std::iter_swap( first, tail );
					++first;
				}
			}
			return DAW_FWD( r );
		}
	};

	struct ReverseView_t {
		explicit ReverseView_t( ) = default;

		template<BidirectionalRange R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return reverse_view<R>{ DAW_FWD( r ) };
		}
	};
} // namespace daw::pipelines::pimpl
namespace daw::pipelines {
	inline constexpr auto Reverse = pimpl::Reverse_t{ };
	inline constexpr auto ReverseView = pimpl::ReverseView_t{ };
} // namespace daw::pipelines
