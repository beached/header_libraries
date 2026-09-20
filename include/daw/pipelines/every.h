#pragma once

// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_algorithm.h"
#include "daw/daw_mutable_capture.h"
#include "daw/pipelines/counted_source.h"
#include "daw/pipelines/move_next.h"
#include "daw/pipelines/view.h"

#include <cstddef>
namespace daw::pipelines::pimpl {
	template<Iterator SentinelFor>
	struct every_iterator_end {
		using iterator_category =
		  daw::common_iterator_category_t<std::forward_iterator_tag,
		                                  daw::iterator_category_t<SentinelFor>>;
		using difference_type = daw::iter_difference_t<SentinelFor>;
		using value_type = std::common_type_t<iter_value_t<SentinelFor>>;
		using reference = std::common_reference_t<iter_reference_t<SentinelFor>>;
		using pointer = void;
		using i_am_a_daw_every_iterator_end_class = void;

		every_iterator_end( ) = default;

		constexpr bool operator==( every_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline every_iterator_end &
		operator++( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline every_iterator_end
		operator++( int ) const {
			std::terminate( );
		}
	};

	template<Iterator First, Iterator Last>
	struct every_iterator {
		using iterator_category =
		  common_iterator_category_t<std::forward_iterator_tag,
		                             daw::iter_category_t<First>>;
		using value_type = daw::iter_value_t<First>;
		using reference = daw::iter_reference_t<First>;
		using const_reference = daw::iter_const_reference_t<First>;
		using pointer = daw::iter_pointer_t<First>;
		using const_pointer = std::remove_pointer_t<pointer> const *;
		using difference_type = daw::iter_difference_t<First>;
		using i_am_a_daw_every_iterator_class = void;

	private:
		First m_first{ };
		DAW_NO_UNIQUE_ADDRESS Last m_last{ };
		std::ptrdiff_t m_every_nth = 1;

	public:
		every_iterator( ) = default;

		explicit constexpr every_iterator( daw::constructible<First> auto &&first,
		                                   daw::constructible<Last> auto &&last,
		                                   std::ptrdiff_t every_nth )
		  : m_first( DAW_FWD( first ) )
		  , m_last( DAW_FWD( last ) )
		  , m_every_nth( every_nth ) {
			daw_ensure( m_every_nth > 0 );
		}

		[[nodiscard]] constexpr auto &base( ) {
			return m_first;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr reference operator*( ) {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *m_first;
		}

		[[nodiscard]] constexpr pointer operator->( ) {
			return std::to_address( m_first );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const {
			return std::to_address( m_first );
		}

		[[nodiscard]] constexpr bool good( ) const {
			return m_first != m_last;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const {
			return good( );
		}

		constexpr every_iterator &operator++( ) {
			m_first = safe_move_next( m_first, m_last, m_every_nth );
			return *this;
		}

		[[nodiscard]] constexpr every_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] constexpr bool operator==( every_iterator const &rhs ) const {
			return m_first == rhs.m_first;
		}

		[[nodiscard]] constexpr bool
		operator==( every_iterator_end<First> const & ) const {
			return not good( );
		}
	};

	/// Over a random access source of known length the selected elements are
	/// counted, so there is no comparison against the end of the source on each
	/// step
	template<Iterator First, Iterator Last>
	struct every_index_iterator {
		using iterator_category =
		  common_iterator_category_t<std::forward_iterator_tag,
		                             daw::iter_category_t<First>>;
		using value_type = daw::iter_value_t<First>;
		using reference = daw::iter_reference_t<First>;
		using const_reference = daw::iter_const_reference_t<First>;
		using pointer = daw::iter_pointer_t<First>;
		using const_pointer = std::remove_pointer_t<pointer> const *;
		using difference_type = daw::iter_difference_t<First>;
		using i_am_a_daw_every_iterator_class = void;

	private:
		First m_first{ };
		std::ptrdiff_t m_idx = 0;   // the selected element we are on
		std::ptrdiff_t m_count = 0; // how many elements are selected
		std::ptrdiff_t m_every_nth = 1;

	public:
		every_index_iterator( ) = default;

		explicit constexpr every_index_iterator(
		  daw::constructible<First> auto &&first,
		  daw::constructible<Last> auto &&last, std::ptrdiff_t every_nth )
		  : m_first( DAW_FWD( first ) )
		  , m_every_nth( every_nth ) {
			daw_ensure( m_every_nth > 0 );
			auto const len = counted_length( m_first, last );
			m_count = len / m_every_nth + ( len % m_every_nth != 0 ? 1 : 0 );
		}

		[[nodiscard]] constexpr First base( ) const {
			return m_first + ( m_idx * m_every_nth );
		}

		[[nodiscard]] constexpr reference operator*( ) {
			return *base( );
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *base( );
		}

		[[nodiscard]] constexpr pointer operator->( ) {
			return std::to_address( base( ) );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const {
			return std::to_address( base( ) );
		}

		[[nodiscard]] constexpr bool good( ) const {
			return m_idx < m_count;
		}

		[[nodiscard]] explicit constexpr operator bool( ) const {
			return good( );
		}

		constexpr every_index_iterator &operator++( ) {
			++m_idx;
			return *this;
		}

		[[nodiscard]] constexpr every_index_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] constexpr bool
		operator==( every_index_iterator const &rhs ) const {
			return m_idx == rhs.m_idx and m_first == rhs.m_first;
		}

		[[nodiscard]] constexpr bool
		operator==( every_iterator_end<First> const & ) const {
			return not good( );
		}
	};

	template<Iterator First, Iterator Last>
	using every_iterator_for =
	  std::conditional_t<counted_source<First, Last>,
	                     every_index_iterator<First, Last>,
	                     every_iterator<First, Last>>;
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<Range R>
	struct every_view : private pimpl::stored_range_base_t<R> {
		using base_t = pimpl::stored_range_base_t<R>;

	private:
		std::ptrdiff_t m_every_nth = 1;

	public:
		using iterator =
		  pimpl::every_iterator_for<iterator_t<R>, iterator_end_t<R>>;
		using const_iterator =
		  pimpl::every_iterator_for<const_iterator_t<R>, const_iterator_end_t<R>>;
		using iterator_last = pimpl::every_iterator_end<iterator_t<R>>;
		using const_iterator_last = pimpl::every_iterator_end<const_iterator_t<R>>;

		explicit every_view( ) = default;

		explicit constexpr every_view( Range auto &&r, std::ptrdiff_t every_nth )
		  : base_t( DAW_FWD( r ) )
		  , m_every_nth( every_nth ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ base_t::rbegin( ), base_t::rend( ), m_every_nth };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{ base_t::rbegin( ), base_t::rend( ), m_every_nth };
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return iterator_last{ };
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return const_iterator_last{ };
		}
	};
	template<typename R>
	every_view( R && ) -> every_view<R>;

} // namespace daw::pipelines
namespace daw::pipelines::pimpl {
	struct Every_t {
		std::ptrdiff_t m_every_nth;

		explicit constexpr Every_t( std::ptrdiff_t every_nth )
		  : m_every_nth( every_nth ) {
			daw_ensure( m_every_nth > 0 );
		}

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			return every_view<R>{ DAW_FWD( r ), m_every_nth };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	/// Given a Range, output every Nth element
	[[nodiscard]] constexpr auto Every( std::ptrdiff_t every_nth ) {
		daw_ensure( every_nth > 0 );
		return pimpl::Every_t{ every_nth };
	}
} // namespace daw::pipelines
