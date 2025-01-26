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
#include "daw/pipelines/range.h"
#include "daw/pipelines/range_base.h"

#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

namespace daw::pipelines {
	namespace pimpl {
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

			constexpr std::ptrdiff_t size( ) {
				return 1;
			}

			constexpr delimiter_length_t( Delimiter const &d )
			  : length( std::distance( std::begin( d ), std::end( d ) ) ) {}
		};
	} // namespace pimpl

	template<ForwardRange R, typename Delimiter, typename Projection>
	struct split_view;

	template<ForwardIterator I, typename IteratorLast, typename Delimiter,
	         typename Projection>
	struct split_view_iterator {
		using iterator_category = std::forward_iterator_tag;
		using value_type = range_t<I, I>;
		using reference = value_type;
		using const_reference = value_type;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_split_view_iterator = void;

	private:
		DAW_NO_UNIQUE_ADDRESS I m_first = I{ };
		DAW_NO_UNIQUE_ADDRESS IteratorLast m_last = IteratorLast{ };
		DAW_NO_UNIQUE_ADDRESS Delimiter m_delimiter{ };
		DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };
		mutable std::optional<I> m_next = std::nullopt;

		constexpr void increment_iterator( ) const {
			if( m_next or m_first == m_last ) {
				return;
			}
			if constexpr( std::is_invocable_v<Delimiter, iter_reference_t<I>> ) {
				m_next = daw::pipelines::FindIf( range_t{ m_first, m_last },
				                                 m_delimiter, m_projection );
			} else if constexpr( Range<Delimiter> ) {
				m_next = std::search( m_first, m_last, std::begin( m_delimiter ),
				                      std::end( m_delimiter ) );
			} else {
				m_next = daw::pipelines::Find( range_t{ m_first, m_last }, m_delimiter,
				                               m_projection );
			}
			assert( m_next );
		}

		constexpr auto get_delimiter_length( ) const {
			return pimpl::delimiter_length_t<Delimiter>{ m_delimiter }.size( );
		}

	public:
		split_view_iterator( ) = default;

		constexpr split_view_iterator( I first, IteratorLast last, Delimiter d,
		                               Projection p )
		  : m_first( std::move( first ) )
		  , m_last( std::move( last ) )
		  , m_delimiter( std::move( d ) )
		  , m_projection( std::move( p ) ) {
			increment_iterator( );
		}

		constexpr split_view_iterator &operator++( ) {
			assert( m_next );
			m_first = std::move( *m_next );
			m_next.reset( );
			if( m_first != m_last ) {
				std::advance( m_first, get_delimiter_length( ) );
			}
			return *this;
		}

		constexpr split_view_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr const_reference operator*( ) const {
			increment_iterator( );
			assert( m_next );
			return const_reference{ m_first, *m_next };
		}

		constexpr bool operator==( split_view_iterator const &rhs ) const {
			return m_first == rhs.m_first and m_last == rhs.m_last and
			       m_next == rhs.m_next;
		}
		constexpr bool operator!=( split_view_iterator const &rhs ) const = default;
	};

	template<ForwardRange R, typename Delimiter,
	         typename Projection = std::identity>
	struct split_view
	  : range_base_t<split_view_iterator<
	      iterator_t<daw::remove_rvalue_ref_t<R>>,
	      iterator_end_t<daw::remove_rvalue_ref_t<R>>, Delimiter, Projection>> {
		using i_am_a_daw_split_view_class = void;

	private:
		using range_type = daw::remove_rvalue_ref_t<R>;
		using iterator =
		  split_view_iterator<iterator_t<range_type>, iterator_end_t<range_type>,
		                      Delimiter, Projection>;

		DAW_NO_UNIQUE_ADDRESS range_type m_range{ };
		DAW_NO_UNIQUE_ADDRESS Delimiter m_delimiter{ };
		DAW_NO_UNIQUE_ADDRESS Projection m_projection = Projection{ };
		DAW_NO_UNIQUE_ADDRESS iterator_t<range_type> m_last =
		  advance_to_last( std::begin( m_range ), std::end( m_range ) );

		static constexpr auto advance_to_last( auto f, auto l ) {
			if constexpr( RandomRange<range_type> ) {
				auto const sz = std::distance( f, l );
				return std::next( f, sz );
			} else {
				while( f != l ) {
					++f;
				}
				return f;
			}
		}

	public:
		split_view( ) = default;

		constexpr split_view( std::convertible_to<range_type> auto &&r, Delimiter d,
		                      Projection p = Projection{ } )
		  : m_range{ DAW_FWD( r ) }
		  , m_delimiter{ std::move( d ) }
		  , m_projection{ std::move( p ) } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ std::begin( m_range ), m_last, m_delimiter,
			                 m_projection };
		}

		[[nodiscard]] constexpr iterator begin( ) const {
			return iterator{ std::begin( m_range ), m_last, m_delimiter,
			                 m_projection };
		}

		[[nodiscard]] constexpr iterator cbegin( ) const {
			return iterator{ std::begin( m_range ), m_last, m_delimiter,
			                 m_projection };
		}

		[[nodiscard]] constexpr iterator end( ) {
			return iterator{ m_last, m_last, m_delimiter, m_projection };
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return iterator{ m_last, m_last, m_delimiter, m_projection };
		}

		[[nodiscard]] constexpr iterator cend( ) const {
			return iterator{ m_last, m_last, m_delimiter, m_projection };
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
				return split_view<R, Delimiter, Projection>( DAW_FWD( r ), m_delimiter,
				                                             m_projection );
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
		return pimpl::split_t{ DAW_FWD( d ), DAW_FWD( p ) };
	}
} // namespace daw::pipelines
