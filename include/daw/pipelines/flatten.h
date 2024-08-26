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

#include <cstddef>
#include <iterator>

namespace daw::pipelines::pimpl {
	template<Range R>
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
		R m_range;
		iterator_t m_range_first{ };
		sub_iterator_t m_cur_first{ };

		DAW_ATTRIB_NOINLINE constexpr void inc_range( ) {
			++m_range_first;
			if( m_range_first == std::end( m_range ) ) {
				m_cur_first = sub_iterator_t{ };
			}
		}

		DAW_ATTRIB_NOINLINE constexpr void inc_sub_range( ) {
			++m_cur_first;
			if( m_cur_first == std::end( *begin( ) ) ) {
				inc_range( );
			}
		}

	public:
		explicit flatten_view( ) = default;

		explicit constexpr flatten_view( R const &r )
		  : m_range( r )
		  , m_range_first( std::begin( m_range ) )
		  , m_cur_first( empty_range( m_range ) ? sub_iterator_t{ }
		                                        : std::begin( *m_range_first ) ) {}

		explicit constexpr flatten_view( R &&r )
		  : m_range( DAW_FWD( r ) )
		  , m_range_first( std::begin( m_range ) )
		  , m_cur_first( empty_range( m_range ) ? sub_iterator_t{ }
		                                        : std::begin( *m_range_first ) ) {}

		[[nodiscard]] constexpr sub_iterator_t begin( ) const {
			return m_cur_first;
		}

		[[nodiscard]] constexpr sub_iterator_t end( ) const {
			return std::end( *m_range_first );
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
			return *m_cur_first;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			return *m_cur_first;
		}

		[[nodiscard]] DAW_ATTRIB_NOINLINE constexpr bool
		operator==( flatten_view const &rhs ) const noexcept {
			return m_range_first == rhs.m_range_first and
			       m_cur_first == rhs.m_cur_first;
		}

		[[nodiscard]] constexpr bool
		operator!=( flatten_view const &rhs ) const noexcept = default;
	};
	template<Range R>
	flatten_view( R ) -> flatten_view<R>;

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
