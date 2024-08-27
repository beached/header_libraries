// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"
#include "daw/daw_traits.h"
#include "pipeline_traits.h"
#include "range.h"

#include <cassert>
#include <cstddef>
#include <iterator>

namespace daw::pipelines::pimpl {
	template<ForwardRange R>
	struct flatten_view {
		using iterator_t = daw::iterator_t<R>;
		using sub_iterator_t = daw::iterator_t<daw::iter_value_t<iterator_t>>;
		using iterator_category =
		  std::forward_iterator_tag; /*
common_iterator_category_t<daw::iter_category_t<iterator_t>,
		     daw::iter_category_t<underlying_iterator_t>>; */
		using value_type = daw::iter_value_t<sub_iterator_t>;
		using reference = daw::iter_reference_t<sub_iterator_t>;
		using const_reference = daw::iter_const_reference_t<sub_iterator_t>;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_flatten_view_class = void;

	private:
		struct end_t {
			using iterator_t = daw::iterator_t<R>;
			using sub_iterator_t = daw::iterator_t<daw::iter_value_t<iterator_t>>;
			using iterator_category = std::forward_iterator_tag;
			using value_type = daw::iter_value_t<sub_iterator_t>;
			using reference = daw::iter_reference_t<sub_iterator_t>;
			using const_reference = daw::iter_const_reference_t<sub_iterator_t>;
			using difference_type = std::ptrdiff_t;

			iterator_t m_range_first{ };
		};

		using m_range_t = daw::remove_cvrvref_t<R>;
		m_range_t m_range;
		iterator_t m_range_first{ };
		sub_iterator_t m_cur_first{ };

		[[nodiscard]] constexpr auto rabegin( ) {
			return std::begin( m_range );
		}

		[[nodiscard]] constexpr auto rabegin( ) const {
			return std::begin( m_range );
		}

		[[nodiscard]] constexpr auto raend( ) {
			return std::end( m_range );
		}

		[[nodiscard]] constexpr auto raend( ) const {
			return std::end( m_range );
		}

		DAW_ATTRIB_INLINE constexpr void inc_range( ) {
			assert( m_range_first != raend( ) );
			++m_range_first;
			if( m_range_first == raend( ) ) {
				m_cur_first = sub_iterator_t{ };
				assert( *this == end( ) );
			} else {
				m_cur_first = std::begin( *m_range_first );
			}
		}

		DAW_ATTRIB_INLINE constexpr void inc_sub_range( ) {
			assert( m_cur_first != sub_iterator_t{ } );
			assert( m_cur_first != std::end( *m_range_first ) );
			++m_cur_first;
			if( m_cur_first == std::end( *m_range_first ) ) {
				inc_range( );
			}
		}

	public:
		explicit flatten_view( ) = default;

		explicit constexpr flatten_view( auto &&r )
		  : m_range( DAW_FWD( r ) )
		  , m_range_first( rabegin( ) )
		  , m_cur_first( m_range_first == raend( )
		                   ? sub_iterator_t{ }
		                   : std::begin( *m_range_first ) ) {}

		[[nodiscard]] constexpr flatten_view begin( ) const {
			return *this;
		}

		[[nodiscard]] constexpr auto end( ) const {
			return end_t{ raend( ) };
		}

		DAW_ATTRIB_NOINLINE constexpr flatten_view &operator++( ) {
			inc_sub_range( );
			return *this;
		}

		[[nodiscard]] constexpr flatten_view operator++( int ) {
			auto tmp = *this;
			++( *this );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) noexcept {
			assert( m_range_first != raend( ) );
			assert( m_cur_first != std::end( *m_range_first ) );
			assert( m_cur_first != sub_iterator_t{ } );
			return *m_cur_first;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			assert( m_range_first != raend( ) );
			assert( m_cur_first != std::end( *m_range_first ) );
			assert( m_cur_first != sub_iterator_t{ } );
			return *m_cur_first;
		}

		[[nodiscard]] DAW_ATTRIB_NOINLINE constexpr bool
		operator==( flatten_view const &rhs ) const noexcept {
			return m_range_first == rhs.m_range_first and
			       m_cur_first == rhs.m_cur_first;
		}

		[[nodiscard]] constexpr bool
		operator!=( flatten_view const &rhs ) const noexcept = default;

		[[nodiscard]] DAW_ATTRIB_NOINLINE constexpr bool
		operator==( end_t const &rhs ) const noexcept {
			return m_range_first == rhs.m_range_first;
		}

		[[nodiscard]] constexpr bool operator!=( end_t const &rhs ) const noexcept {
			return m_range_first != rhs.m_range_first;
		}
	};
	template<Range R>
	flatten_view( R && ) -> flatten_view<R>;

	struct Flatten_t {
		[[nodiscard]] DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( Range auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return flatten_view{ DAW_FWD( r ) };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	inline constexpr auto Flatten = pimpl::Flatten_t{ };
} // namespace daw::pipelines
