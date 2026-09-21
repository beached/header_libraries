// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_algorithm.h"
#include "daw/daw_as.h"
#include "daw/daw_attributes.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"
#include "daw/pipelines/counted_source.h"
#include "daw/pipelines/move_next.h"
#include "daw/pipelines/view.h"

#include <algorithm>
#include <concepts>
#include <cstddef>

namespace daw::pipelines::pimpl {
	template<Iterator SentinelFor>
	struct slide_iterator_end {
		using iterator_category =
		  daw::common_iterator_category_t<std::forward_iterator_tag,
		                                  daw::iterator_category_t<SentinelFor>>;
		using difference_type = daw::iter_difference_t<SentinelFor>;
		using value_type = std::common_type_t<iter_value_t<SentinelFor>>;
		using reference = std::common_reference_t<iter_reference_t<SentinelFor>>;
		using pointer = void;
		using i_am_a_daw_slide_iterator_end_class = void;

		slide_iterator_end( ) = default;

		constexpr bool operator==( slide_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline slide_iterator_end &
		operator++( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline slide_iterator_end
		operator++( int ) const {
			std::terminate( );
		}
	};

	template<typename First, typename Last = First>
	struct slide_iterator {
		using iterator = First;
		using const_iterator = iterator;
		using iterator_category = std::forward_iterator_tag;
		using value_type = view_t<iterator, Last>;
		using reference = value_type;
		using const_reference = value_type;
		using difference_type = daw::iter_difference_t<iterator>;
		using i_am_a_daw_slide_iterator_class = void;

	private:
		iterator m_iter{ };
		difference_type m_slide_size = 1;
		DAW_NO_UNIQUE_ADDRESS Last m_last{ };
		mutable iterator m_next_iter = m_iter;

	public:
		explicit slide_iterator( ) = default;

		explicit constexpr slide_iterator( iterator first, Last last )
		  : m_iter{ std::move( first ) }
		  , m_last{ std::move( last ) }
		  , m_next_iter( safe_move_next( m_iter, m_last, m_slide_size ) ) {}

		explicit constexpr slide_iterator( iterator first, Last last,
		                                   std::size_t slide_size )
		  : m_iter{ std::move( first ) }
		  , m_slide_size( as<difference_type>( slide_size ) )
		  , m_last{ std::move( last ) }
		  , m_next_iter( safe_move_next( m_iter, m_last, m_slide_size ) ) {
			daw_ensure( m_slide_size > 0 );
		}

		[[nodiscard]] constexpr iterator &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr const_iterator &base( ) const {
			return m_iter;
		}

		constexpr slide_iterator &operator++( ) {
			if( m_next_iter == m_last ) {
				m_iter = m_last;
				return *this;
			}
			++m_iter;
			++m_next_iter;
			return *this;
		}

		[[nodiscard]] constexpr slide_iterator operator++( int ) {
			auto tmp = *this;
			this->operator++( );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) {
			return view_t<iterator>{ m_iter, m_next_iter };
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return view_t<iterator>{ m_iter, m_next_iter };
		}

		[[nodiscard]] constexpr bool operator==( slide_iterator const &rhs ) const {
			return m_iter == rhs.m_iter and m_slide_size == rhs.m_slide_size;
		}

		[[nodiscard]] constexpr bool
		operator==( slide_iterator_end<First> const & ) const {
			return m_iter == m_last;
		}
	};

	/// A random access source with a sized end knows how many windows there are
	/// up front, so the iterator only needs the window start, how many windows
	/// are left and the window length.  There is no per step comparison against
	/// the end of the source, which lets the compiler see a counted loop.
	template<typename First, typename Last = First>
	struct slide_index_iterator {
		using iterator = First;
		using const_iterator = iterator;
		using iterator_category = std::forward_iterator_tag;
		using value_type = view_t<iterator>;
		using reference = value_type;
		using const_reference = value_type;
		using difference_type = daw::iter_difference_t<iterator>;
		using i_am_a_daw_slide_iterator_class = void;

	private:
		iterator m_iter{ };
		difference_type m_count = 0;  // windows left
		difference_type m_window = 0; // elements in each window

	public:
		slide_index_iterator( ) = default;

		explicit constexpr slide_index_iterator( iterator first, Last last )
		  : slide_index_iterator( std::move( first ), std::move( last ),
		                          std::size_t{ 1 } ) {}

		explicit constexpr slide_index_iterator( iterator first, Last last,
		                                         std::size_t slide_size )
		  : m_iter{ std::move( first ) } {
			daw_ensure( slide_size > 0 );
			auto const n = as<difference_type>( slide_size );
			auto const len = counted_length( m_iter, last );
			m_window = std::min( n, len );
			// a range shorter than the window gets one short window, an empty
			// range gets none
			m_count = len == 0 ? 0 : ( len >= n ? len - n + 1 : 1 );
		}

		[[nodiscard]] constexpr iterator &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr const_iterator &base( ) const {
			return m_iter;
		}

		constexpr slide_index_iterator &operator++( ) {
			++m_iter;
			--m_count;
			return *this;
		}

		[[nodiscard]] constexpr slide_index_iterator operator++( int ) {
			auto tmp = *this;
			this->operator++( );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) {
			return view_t<iterator>{ m_iter, m_iter + m_window };
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return view_t<iterator>{ m_iter, m_iter + m_window };
		}

		[[nodiscard]] constexpr bool
		operator==( slide_index_iterator const &rhs ) const {
			return ( m_count <= 0 and rhs.m_count <= 0 ) or m_iter == rhs.m_iter;
		}

		[[nodiscard]] constexpr bool
		operator==( slide_iterator_end<First> const & ) const {
			return m_count <= 0;
		}
	};

	template<typename First, typename Last>
	using slide_iterator_for =
	  std::conditional_t<counted_source<First, Last>,
	                     slide_index_iterator<First, Last>,
	                     slide_iterator<First, Last>>;

	template<ForwardRange R>
	struct slide_view
	  : private stored_range_base_t<
	      R, slide_iterator_for<iterator_t<R>, iterator_end_t<R>>,
	      slide_iterator_end<iterator_t<R>>,
	      slide_iterator_for<const_iterator_t<R>, const_iterator_end_t<R>>,
	      slide_iterator_end<const_iterator_t<R>>> {

		using base_t = stored_range_base_t<
		  R, slide_iterator_for<iterator_t<R>, iterator_end_t<R>>,
		  slide_iterator_end<iterator_t<R>>,
		  slide_iterator_for<const_iterator_t<R>, const_iterator_end_t<R>>,
		  slide_iterator_end<const_iterator_t<R>>>;

		using iterator = typename base_t::iterator_first_t;
		using const_iterator = typename base_t::const_iterator_first_t;
		using last_iterator = typename base_t::iterator_last_t;
		using const_last_iterator = typename base_t::const_iterator_last_t;
		using iterator_category = range_category_t<R>;
		using i_am_a_daw_slide_view_class = void;

	private:
		std::size_t m_slide_size = 1;

	public:
		explicit slide_view( ) = default;

		explicit constexpr slide_view( daw::constructible<base_t> auto &&r )
		  requires(
		    not std::same_as<std::remove_cvref_t<decltype( r )>, slide_view> )
		  : base_t{ DAW_FWD( r ) } {}

		explicit constexpr slide_view( daw::constructible<base_t> auto &&r,
		                               std::size_t slide_size )
		  : base_t{ DAW_FWD( r ) }
		  , m_slide_size{ slide_size } {
			daw_ensure( m_slide_size > 0 );
		}

		constexpr iterator begin( ) {
			return iterator{ base_t::rbegin( ), base_t::rend( ), m_slide_size };
		}

		constexpr const_iterator begin( ) const {
			return const_iterator{ base_t::rbegin( ), base_t::rend( ), m_slide_size };
		}

		constexpr last_iterator end( ) {
			return last_iterator{ };
		}

		constexpr const_last_iterator end( ) const {
			return const_last_iterator{ };
		}

		[[nodiscard]] constexpr bool
		operator==( slide_view const &rhs ) const = default;

		[[nodiscard]] constexpr bool
		operator!=( slide_view const &rhs ) const = default;
	};
	template<Range R>
	slide_view( R &&r ) -> slide_view<daw::remove_rvalue_ref_t<R>>;

	template<Range R>
	slide_view( R &&r, std::size_t ) -> slide_view<daw::remove_rvalue_ref_t<R>>;

	struct Slide_t {
		std::size_t slide_size = 1;
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
		operator( )( ForwardRange auto &&r ) const {
			return slide_view{ DAW_FWD( r ), slide_size };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	Slide( std::size_t slide_size ) {
		return pimpl::Slide_t{ slide_size };
	}
} // namespace daw::pipelines
