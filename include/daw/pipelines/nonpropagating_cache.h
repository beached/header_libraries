// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_ref_storage.h"

#include <optional>
#include <type_traits>
#include <utility>

namespace daw {
	/// A cache that is never copied or moved.  Copying or moving it gives an
	/// empty cache, moving also empties the source, and assigning to it empties
	/// it.  A view can then cache something like begin( ) that would be invalid
	/// in a copy of the view.
	/// Filling it requires a non-const cache, as filling a const one would not
	/// be safe when used from several threads
	template<typename T>
	class nonpropagating_cache {
		using storage_t = daw::ref_storage<T>;
		std::optional<storage_t> m_value{ };

	public:
		nonpropagating_cache( ) = default;

		constexpr ~nonpropagating_cache( ) = default;

		constexpr nonpropagating_cache( nonpropagating_cache const & ) noexcept {}

		constexpr nonpropagating_cache( nonpropagating_cache &&other ) noexcept {
			other.reset( );
		}

		constexpr nonpropagating_cache &
		operator=( nonpropagating_cache const &rhs ) noexcept {
			if( this != &rhs ) {
				reset( );
			}
			return *this;
		}

		constexpr nonpropagating_cache &
		operator=( nonpropagating_cache &&rhs ) noexcept {
			reset( );
			rhs.reset( );
			return *this;
		}

		/// All caches compare equal, so a view holding one can default its
		/// operator==
		[[nodiscard]] friend constexpr bool
		operator==( nonpropagating_cache const &,
		            nonpropagating_cache const & ) noexcept {
			return true;
		}

		[[nodiscard]] constexpr bool has_value( ) const noexcept {
			return m_value.has_value( );
		}

		[[nodiscard]] explicit constexpr operator bool( ) const noexcept {
			return has_value( );
		}

		constexpr void reset( ) noexcept {
			m_value.reset( );
		}

		template<typename F>
		requires( daw::constructible<std::invoke_result_t<F>, storage_t> )
		constexpr decltype( auto ) set_if_empty_get( F &&fn ) & {
			if( not m_value ) {
				m_value.emplace( DAW_FWD( fn )( ) );
			}
			return get( );
		}

		template<typename F>
		requires( daw::constructible<std::invoke_result_t<F>, storage_t> )
		constexpr decltype( auto ) set_if_empty_get( F &&fn ) && {
			if( not m_value ) {
				m_value.emplace( DAW_FWD( fn )( ) );
			}
			return std::move( *this ).get( );
		}

		/// Precondition: has_value( )
		[[nodiscard]] constexpr decltype( auto ) get( ) & {
			return m_value->get( );
		}

		[[nodiscard]] constexpr decltype( auto ) get( ) const & {
			return m_value->get( );
		}

		[[nodiscard]] constexpr decltype( auto ) get( ) && {
			return std::move( *m_value ).get( );
		}

		[[nodiscard]] constexpr decltype( auto ) get( ) const && {
			return std::move( *m_value ).get( );
		}
	};
} // namespace daw
