// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/cpp_17.h"
#include "daw/daw_attributes.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"
#include "daw/pipelines/view.h"

#include <cstddef>

namespace daw::pipelines::pimpl {
	template<typename First, typename Last = First>
	struct chunk_iterator {
		using iterator = First;
		using const_iterator = iterator;
		using iterator_category = std::forward_iterator_tag;
		using value_type = view_t<iterator>;
		using reference = value_type;
		using const_reference = value_type;
		using difference_type = daw::iter_difference_t<iterator>;
		using i_am_a_daw_chunk_iterator_class = void;

	private:
		iterator m_iter{ };
		DAW_NO_UNIQUE_ADDRESS Last m_last{ };
		difference_type m_chunk_size = 1;
		mutable std::optional<iterator> m_next_iter{ };

		[[nodiscard]] constexpr iterator find_next_iter( ) const {
			auto result = m_iter;
			auto n = m_chunk_size;
			while( n > 0 and result != m_last ) {
				--n;
				++result;
			}
			return result;
		}

		constexpr void increment( ) {
			if( m_next_iter ) {
				m_iter = std::move( *m_next_iter );
				m_next_iter.reset( );
				return;
			}
			m_iter = find_next_iter( );
		}

	public:
		explicit chunk_iterator( ) = default;

		explicit constexpr chunk_iterator( iterator first )
		  : m_iter{ first }
		  , m_last{ first }
		  , m_chunk_size{ 1 } {}

		explicit constexpr chunk_iterator( iterator first, Last last,
		                                   std::size_t chunk_size )
		  : m_iter{ first }
		  , m_last{ last }
		  , m_chunk_size{ static_cast<difference_type>( chunk_size ) } {}

		[[nodiscard]] constexpr iterator &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr const_iterator &base( ) const {
			return m_iter;
		}

		constexpr chunk_iterator &operator++( ) {
			increment( );
			return *this;
		}

		[[nodiscard]] constexpr chunk_iterator operator++( int ) {
			auto tmp = *this;
			increment( );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) {
			m_next_iter = find_next_iter( );
			return view_t<iterator>{ m_iter, *m_next_iter };
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			m_next_iter = find_next_iter( );
			return view_t<iterator>{ m_iter, *m_next_iter };
		}

		[[nodiscard]] constexpr bool
		operator==( chunk_iterator const &rhs ) const noexcept {
			return m_iter == rhs.m_iter;
		}

		[[nodiscard]] constexpr bool
		operator!=( chunk_iterator const &rhs ) const noexcept = default;
	};

	template<ForwardRange R>
	struct chunk_view
	  : private stored_range_base_t<
	      R, chunk_iterator<iterator_t<R>, iterator_end_t<R>>,
	      chunk_iterator<iterator_end_t<R>>,
	      chunk_iterator<const_iterator_t<R>, const_iterator_end_t<R>>,
	      chunk_iterator<const_iterator_end_t<R>>> {

		using base_t = stored_range_base_t<
		  R, chunk_iterator<iterator_t<R>, iterator_end_t<R>>,
		  chunk_iterator<iterator_end_t<R>>,
		  chunk_iterator<const_iterator_t<R>, const_iterator_end_t<R>>,
		  chunk_iterator<const_iterator_end_t<R>>>;

		using iterator = typename base_t::iterator_first_t;
		using const_iterator = typename base_t::const_iterator_first_t;
		using last_iterator = typename base_t::iterator_last_t;
		using const_last_iterator = typename base_t::const_iterator_last_t;
		using iterator_category = range_category_t<R>;
		using i_am_a_daw_chunk_view_class = void;

	private:
		std::size_t m_chunk_size = 1;

	public:
		explicit chunk_view( ) = default;

		explicit chunk_view( R r )
		  : base_t{ DAW_FWD( r ) } {}

		explicit constexpr chunk_view( R r, std::size_t chunk_size )
		  : base_t{ DAW_FWD( r ) }
		  , m_chunk_size{ chunk_size } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ base_t::rbegin( ), base_t::rend( ), m_chunk_size };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{ base_t::rbegin( ), base_t::rend( ), m_chunk_size };
		}

		[[nodiscard]] constexpr last_iterator end( ) {
			return last_iterator{ base_t::rend( ), base_t::rend( ), m_chunk_size };
		}

		[[nodiscard]] constexpr const_last_iterator end( ) const {
			return const_last_iterator{
			  base_t::rend( ), base_t::rend( ), m_chunk_size };
		}

		[[nodiscard]] constexpr bool
		operator==( chunk_view const &rhs ) const = default;

		[[nodiscard]] constexpr bool
		operator!=( chunk_view const &rhs ) const = default;
	};
	template<Range R>
	chunk_view( R &&r ) -> chunk_view<daw::remove_rvalue_ref_t<R>>;
	template<Range R>
	chunk_view( R &&r, std::size_t ) -> chunk_view<daw::remove_rvalue_ref_t<R>>;

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
