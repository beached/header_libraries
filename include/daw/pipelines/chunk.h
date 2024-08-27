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
#include "daw/daw_remove_cvref.h"
#include "range.h"

#include <cstddef>

namespace daw::pipelines::pimpl {
	template<ForwardRange R>
	struct chunk_view {
		using iterator = daw::iterator_t<R>;
		using const_iterator = daw::iterator_t<std::add_const_t<R>>;
		using iterator_category = daw::range_category_t<R>;
		using value_type = range_t<iterator>;
		using reference = range_t<iterator>;
		using const_reference = range_t<iterator>;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_chunk_view_iterator_class = void;

	private:
		using m_range_t = daw::remove_cvrvref_t<R>;
		m_range_t m_range{ };
		iterator m_iter{ };
		difference_type m_chunk_size = 1;

		[[nodiscard]] constexpr auto ra_end( ) {
			return std::end( m_range );
		}

		[[nodiscard]] constexpr auto ra_end( ) const {
			return std::end( m_range );
		}

		constexpr void increment( ) {
			auto const last = ra_end( );
			auto n = m_chunk_size;
			while( n > 0 and m_iter != last ) {
				--n;
				++m_iter;
			}
		}

	public:
		explicit chunk_view( ) = default;

		explicit chunk_view( Range auto &&r )
		  : m_range{ DAW_FWD( r ) }
		  , m_iter{ std::begin( m_range ) } {}

		explicit constexpr chunk_view( Range auto &&r, std::size_t chunk_size )
		  : m_range{ DAW_FWD( r ) }
		  , m_iter{ std::begin( m_range ) }
		  , m_chunk_size{ static_cast<difference_type>( chunk_size ) } {}

		[[nodiscard]] constexpr iterator &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr const_iterator &base( ) const {
			return m_iter;
		}

		constexpr chunk_view begin( ) {
			return *this;
		}

		constexpr chunk_view begin( ) const {
			return *this;
		}

		constexpr chunk_view end( ) {
			auto result = *this;
			result.m_iter = ra_end( );
			return result;
		}

		constexpr chunk_view end( ) const {
			auto result = *this;
			result.m_iter = ra_end( );
			return result;
		}

		constexpr chunk_view &operator++( ) {
			increment( );
			return *this;
		}

		[[nodiscard]] constexpr chunk_view operator++( int ) {
			auto tmp = *this;
			increment( );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) noexcept {
			return range_t<iterator>{ m_iter, std::next( m_iter, m_chunk_size ) };
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			return range_t<iterator>{ m_iter, std::next( m_iter, m_chunk_size ) };
		}

		[[nodiscard]] constexpr bool
		operator==( chunk_view const &rhs ) const noexcept {
			return m_iter == rhs.m_iter;
		}

		[[nodiscard]] constexpr bool
		operator!=( chunk_view const &rhs ) const noexcept = default;
	};
	template<Range R>
	chunk_view( R &&r ) -> chunk_view<R>;
	template<Range R>
	chunk_view( R &&r, std::size_t ) -> chunk_view<R>;

	struct Chunk_t {
		std::size_t chunk_size;
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
		operator( )( ForwardRange auto &&r ) const {
			return chunk_view{ DAW_FWD( r ), chunk_size };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	Chunk( std::size_t chunk_size ) {
		return pimpl::Chunk_t{ chunk_size };
	}
} // namespace daw::pipelines
