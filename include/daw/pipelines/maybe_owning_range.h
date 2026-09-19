// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_move.h"
#include "daw/daw_ref_storage.h"
#include "daw/pipelines/view.h"

#include <concepts>

namespace daw::pipelines {
	template<Range R>
	struct maybe_owning_range {
		using iterator = daw::iterator_t<R>;
		using const_iterator = daw::const_iterator_t<R>;
		using iterator_last = daw::iterator_end_t<R>;
		using const_iterator_last = daw::const_iterator_end_t<R>;
		using i_am_a_daw_maybe_owning_range = void;

	private:
		using storage_t = daw::ref_storage<R>;
		storage_t m_storage{ };

	public:
		static constexpr bool is_owned = storage_t::is_owned;

		[[nodiscard]] constexpr auto &get_range( ) &
		  requires( not std::is_const_v<std::remove_reference_t<R>> ) {
			return m_storage.get( );
		}

		[[nodiscard]] constexpr auto const &get_range( ) const & {
			return m_storage.get( );
		}

		explicit maybe_owning_range( ) = default;

		explicit constexpr maybe_owning_range(
		  daw::constructible<storage_t> auto &&r )
		  : m_storage( DAW_FWD( r ) ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ std::begin( m_storage.get( ) ) };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{ std::begin( m_storage.get( ) ) };
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return iterator_last{ std::end( m_storage.get( ) ) };
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return const_iterator_last{ std::end( m_storage.get( ) ) };
		}

		[[nodiscard]] constexpr bool
		operator==( maybe_owning_range const &rhs ) const = default;
	};
} // namespace daw::pipelines
