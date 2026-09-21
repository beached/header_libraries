// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/pipelines/find.h"
#include "daw/pipelines/maybe_owning_range.h"
#include "daw/pipelines/range_base.h"
#include "daw/pipelines/view.h"

#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

namespace daw::pipelines::pimpl {
	template<typename Delimiter>
	struct delimiter_length_t {
		static constexpr std::ptrdiff_t size( ) {
			return 1;
		}

		constexpr delimiter_length_t( Delimiter const & ) noexcept {}
	};

	template<ForwardRange Delimiter>
	struct delimiter_length_t<Delimiter> {
		std::ptrdiff_t length;

		explicit constexpr delimiter_length_t( Delimiter const &d )
		  : length{ pimpl::ranges_distance( d ) } {}

		static constexpr std::ptrdiff_t size( ) {
			return 1;
		}
	};

	template<Iterator SentinelFor>
	struct split_iterator_end {
		using iterator_category =
		  daw::common_iterator_category_t<std::forward_iterator_tag,
		                                  daw::iterator_category_t<SentinelFor>>;
		using difference_type = daw::iter_difference_t<SentinelFor>;
		using value_type = std::common_type_t<iter_value_t<SentinelFor>>;
		using reference = std::common_reference_t<iter_reference_t<SentinelFor>>;
		using pointer = void;
		using i_am_a_daw_split_iterator_end_class = void;

		split_iterator_end( ) = default;

		constexpr bool operator==( split_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline split_iterator_end &
		operator++( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline split_iterator_end
		operator++( int ) const {
			std::terminate( );
		}
	};

	template<ForwardIterator First, typename IteratorLast, typename Delimiter,
	         typename Projection>
	struct split_iterator {
		using iterator_category = std::forward_iterator_tag;
		using value_type = view_t<First, First>;
		using reference = value_type;
		using const_reference = value_type;
		using difference_type = daw::iter_difference_t<First>;
		using i_am_a_daw_split_iterator_class = void;

	private:
		First m_first = First{ };
		IteratorLast m_last = IteratorLast{ };
		DAW_NO_UNIQUE_ADDRESS Delimiter m_delimiter{ };
		DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };
		mutable std::optional<First> m_next = std::nullopt;

		constexpr void increment_iterator( ) const {
			if( m_next or m_first == m_last ) {
				return;
			}
			if constexpr( std::is_invocable_v<Delimiter, iter_reference_t<First>> ) {
				m_next = daw::pipelines::FindIf(
				  view_t{ m_first, m_last }, m_delimiter, m_projection );
			} else if constexpr( Range<Delimiter> ) {
				m_next = std::search(
				  m_first, m_last, std::begin( m_delimiter ), std::end( m_delimiter ) );
			} else {
				m_next = daw::pipelines::Find(
				  view_t{ m_first, m_last }, m_delimiter, m_projection );
			}
			assert( m_next );
		}

		constexpr auto get_delimiter_length( ) const {
			return pimpl::delimiter_length_t<Delimiter>{ m_delimiter }.size( );
		}

	public:
		split_iterator( ) = default;

		explicit constexpr split_iterator( First first, IteratorLast last,
		                                   Delimiter d, Projection p )
		  : m_first( std::move( first ) )
		  , m_last( std::move( last ) )
		  , m_delimiter( std::move( d ) )
		  , m_projection( std::move( p ) ) {
			increment_iterator( );
		}

		constexpr split_iterator &operator++( ) {
			increment_iterator( );
			assert( m_next );
			m_first = std::move( *m_next );
			m_next.reset( );
			if( m_first != m_last ) {
				std::advance( m_first, get_delimiter_length( ) );
			}
			return *this;
		}

		[[nodiscard]] constexpr split_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			increment_iterator( );
			assert( m_next );
			return const_reference{ m_first, *m_next };
		}

		[[nodiscard]] constexpr bool good( ) const {
			return m_first != m_last;
		}

		[[nodiscard]] constexpr bool operator==( split_iterator const &rhs ) const {
			return m_first == rhs.m_first and m_last == rhs.m_last and
			       m_next == rhs.m_next;
		}

		[[nodiscard]] constexpr bool
		operator==( split_iterator_end<First> const & ) const {
			return not good( );
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<ForwardRange R, typename Delimiter,
	         typename Projection = std::identity>
	struct split_view : private maybe_owning_range<R> {
		using i_am_a_daw_split_view_class = void;

	private:
		using base_t = maybe_owning_range<R>;
		using iterator = pimpl::split_iterator<iterator_t<R>, iterator_end_t<R>,
		                                       Delimiter, Projection>;
		using iterator_last = pimpl::split_iterator_end<iterator_t<R>>;
		using const_iterator =
		  pimpl::split_iterator<const_iterator_t<R>, const_iterator_end_t<R>,
		                        Delimiter, Projection>;
		using const_iterator_last = pimpl::split_iterator_end<const_iterator_t<R>>;

		DAW_NO_UNIQUE_ADDRESS Delimiter m_delimiter{ };
		DAW_NO_UNIQUE_ADDRESS Projection m_projection = Projection{ };

		static constexpr auto advance_to_last( auto f, auto l ) {
			if constexpr( std::same_as<decltype( f ), decltype( l )> ) {
				// common range, the end is already an iterator
				return l;
			} else if constexpr( RandomRange<R> ) {
				auto const sz = pimpl::ranges_distance( f, l );
				return std::next( f, sz );
			} else {
				while( f != l ) {
					++f;
				}
				return f;
			}
		}

		// Computed on demand instead of cached at construction, so a copied or
		// moved split_view never holds an iterator into another object.
		[[nodiscard]] constexpr auto last( ) {
			return advance_to_last( base_t::begin( ), base_t::end( ) );
		}

		[[nodiscard]] constexpr auto last( ) const {
			return advance_to_last( base_t::begin( ), base_t::end( ) );
		}

	public:
		split_view( ) = default;

		constexpr split_view( constructible<base_t> auto &&r, Delimiter d,
		                      Projection p = Projection{ } )
		  : base_t{ DAW_FWD( r ) }
		  , m_delimiter{ std::move( d ) }
		  , m_projection{ std::move( p ) } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ base_t::begin( ), last( ), m_delimiter, m_projection };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{
			  base_t::begin( ), last( ), m_delimiter, m_projection };
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const {
			return const_iterator{
			  base_t::begin( ), last( ), m_delimiter, m_projection };
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return iterator_last{ };
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return const_iterator_last{ };
		}

		[[nodiscard]] constexpr const_iterator_last cend( ) const {
			return const_iterator_last{ };
		}
	};
	template<ForwardRange R, typename Delimiter>
	split_view( R, Delimiter ) -> split_view<R, Delimiter>;

	template<ForwardRange R, typename Delimiter, typename Projection>
	split_view( R, Delimiter, Projection )
	  -> split_view<R, Delimiter, Projection>;

	static_assert( requires( split_view<daw::string_view, char> r ) {
		std::begin( r );
		std::end( r );
	} );

	namespace pimpl {
		template<typename Delimiter, typename Projection = std::identity>
		struct split_t {
			DAW_NO_UNIQUE_ADDRESS Delimiter m_delimiter;
			DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };

			template<ForwardRange R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				static_assert( std::invocable<Projection, range_value_t<R>>,
				               "Projection must be invocable with range_value_t<R>" );
				return split_view<R, Delimiter, Projection>(
				  DAW_FWD( r ), m_delimiter, m_projection );
			}
		};
		template<typename F>
		split_t( F ) -> split_t<F>;

		template<typename F, typename P>
		split_t( F, P ) -> split_t<F, P>;

	} // namespace pimpl

	template<typename Delimiter, typename Projection = std::identity>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	Split( Delimiter &&d, Projection &&p = Projection{ } ) {
		return pimpl::split_t<daw::remove_rvalue_ref_t<Delimiter>,
		                      daw::remove_rvalue_ref_t<Projection>>{ DAW_FWD( d ),
		                                                             DAW_FWD( p ) };
	}
} // namespace daw::pipelines
