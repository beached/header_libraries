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
#include "daw/pipelines/pipeline_traits.h"
#include "daw/pipelines/view.h"

#include <cassert>
#include <cstddef>
#include <iterator>

namespace daw::pipelines::pimpl {
	template<typename It>
	struct flatten_iterator_end_t {
		using iterator_t = It;
		using sub_iterator_t = daw::iterator_t<daw::iter_reference_t<iterator_t>>;
		using iterator_category = std::forward_iterator_tag;
		using value_type = daw::iter_value_t<sub_iterator_t>;
		using reference = daw::iter_reference_t<sub_iterator_t>;
		using const_reference = daw::iter_const_reference_t<sub_iterator_t>;
		using difference_type = std::ptrdiff_t;

		iterator_t m_range_first{ };
	};

	template<typename First, typename Last = First>
	struct flatten_iterator {
		using iterator_t = First;
		using sub_iterator_t = daw::iterator_t<daw::iter_value_t<iterator_t>>;
		using iterator_category = std::forward_iterator_tag;
		using value_type = daw::iter_value_t<sub_iterator_t>;
		using reference = daw::iter_reference_t<sub_iterator_t>;
		using const_reference = daw::iter_const_reference_t<sub_iterator_t>;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_flatten_iterator_class = void;

	private:
		using end_t = flatten_iterator_end_t<Last>;

		iterator_t m_iter{ };
		DAW_NO_UNIQUE_ADDRESS Last m_last{ };
		sub_iterator_t m_cur_first{ };

		DAW_ATTRIB_INLINE constexpr void advance_to_non_empty_range( ) {
			while( m_iter != m_last ) {
				m_cur_first = std::begin( *m_iter );
				if( m_cur_first != std::end( *m_iter ) ) {
					return;
				}
				++m_iter;
			}
			m_cur_first = sub_iterator_t{ };
		}

		DAW_ATTRIB_INLINE constexpr void inc_range( ) {
			assert( m_iter != m_last );
			++m_iter;
			if( m_iter == m_last ) {
				m_cur_first = sub_iterator_t{ };
			} else {
				m_cur_first = std::begin( *m_iter );
			}
		}

		DAW_ATTRIB_INLINE constexpr void inc_sub_range( ) {
			assert( m_cur_first != sub_iterator_t{ } );
			assert( m_cur_first != std::end( *m_iter ) );
			++m_cur_first;
			if( m_cur_first == std::end( *m_iter ) ) {
				++m_iter;
				advance_to_non_empty_range( );
			}
		}

	public:
		explicit flatten_iterator( ) = default;

		explicit constexpr flatten_iterator( First first, Last last )
		  : m_iter( first )
		  , m_last( last )
		  , m_cur_first( m_iter == m_last ? sub_iterator_t{ }
		                                  : std::begin( *m_iter ) ) {
			advance_to_non_empty_range( );
		}

		constexpr flatten_iterator &operator++( ) {
			inc_sub_range( );
			return *this;
		}

		[[nodiscard]] constexpr flatten_iterator operator++( int ) {
			auto tmp = *this;
			++( *this );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) noexcept {
			assert( m_iter != m_last );
			assert( m_cur_first != std::end( *m_iter ) );
			assert( m_cur_first != sub_iterator_t{ } );
			return *m_cur_first;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			assert( m_iter != m_last );
			assert( m_cur_first != std::end( *m_iter ) );
			assert( m_cur_first != sub_iterator_t{ } );
			return *m_cur_first;
		}

		[[nodiscard]] constexpr bool
		operator==( flatten_iterator const &rhs ) const {
			return m_iter == rhs.m_iter and m_cur_first == rhs.m_cur_first;
		}
		/*
		    [[nodiscard]] constexpr bool
		    operator!=( flatten_iterator const &rhs ) const = default;
		*/
		[[nodiscard]] constexpr bool operator==( end_t const &rhs ) const {
			return m_iter == rhs.m_range_first;
		}
		/*
		    [[nodiscard]] constexpr bool operator!=( end_t const &rhs ) const {
		      return m_iter != rhs.m_range_first;
		    }*/
	};

	template<ForwardRange R>
	struct flatten_view
	  : private stored_range_base_t<R, flatten_iterator<iterator_t<R>>,
	                                flatten_iterator<iterator_end_t<R>>,
	                                flatten_iterator<const_iterator_t<R>>,
	                                flatten_iterator<const_iterator_end_t<R>>> {
		using base_t =
		  stored_range_base_t<R, flatten_iterator<iterator_t<R>>,
		                      flatten_iterator<iterator_end_t<R>>,
		                      flatten_iterator<const_iterator_t<R>>,
		                      flatten_iterator<const_iterator_end_t<R>>>;

		using iterator = typename base_t::iterator_first_t;
		using const_iterator = typename base_t::const_iterator_first_t;
		using last_iterator = typename base_t::iterator_last_t;
		using const_last_iterator = typename base_t::const_iterator_last_t;
		using iterator_category = range_category_t<R>;
		using i_am_a_daw_flatten_view_class = void;
		explicit flatten_view( ) = default;

		template<Range U>
		requires( not std::same_as<std::remove_cvref_t<U>,
		                           flatten_view> ) //
		  explicit constexpr flatten_view( U &&r )
		  : base_t( DAW_FWD( r ) ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ base_t::rbegin( ), base_t::rend( ) };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{ base_t::rbegin( ), base_t::rend( ) };
		}

		[[nodiscard]] constexpr last_iterator end( ) {
			return last_iterator{ base_t::rend( ), base_t::rend( ) };
		}

		[[nodiscard]] constexpr const_last_iterator end( ) const {
			return const_last_iterator{ base_t::rend( ), base_t::rend( ) };
		}

	private:
		using underlying_range_t = std::remove_cvref_t<R>;
		using underlying_value_t = daw::range_value_t<underlying_range_t>;

	public:
		[[nodiscard]] constexpr bool operator==( flatten_view const &rhs ) const
		  requires( std::equality_comparable<underlying_range_t>
		              and std::equality_comparable<underlying_value_t> ) {
			return static_cast<base_t const &>( *this ) ==
			       static_cast<base_t const &>( rhs );
		}

		[[nodiscard]] constexpr bool
		operator!=( flatten_view const &rhs ) const = default;
	};
	template<Range R>
	flatten_view( R && ) -> flatten_view<daw::remove_rvalue_ref_t<R>>;

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
