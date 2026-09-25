// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/iterator/daw_reverse_iterator.h"
#include "daw/pipelines/counted_source.h"
#include "daw/pipelines/maybe_owning_range.h"
#include "daw/pipelines/move_next.h"
#include "daw/pipelines/view.h"

#include <utility>

namespace daw::pipelines::pimpl {
	/// Reverse iterator for a random access source of known length.  It holds the
	/// start of the source and the index of the current element, so a reverse
	/// loop is a counted loop
	template<Iterator First>
	struct reverse_index_iterator {
		using value_type = daw::iter_value_t<First>;
		using difference_type = daw::iter_difference_t<First>;
		using size_type = std::size_t;
		using pointer = daw::iter_pointer_t<First>;
		using reference = daw::iter_reference_t<First>;
		using iterator_category = daw::iter_category_t<First>;
		using i_am_a_daw_reverse_iterator = void;

	private:
		First m_first{ };
		difference_type m_pos = -1; // index of the current element

	public:
		reverse_index_iterator( ) = default;

		explicit constexpr reverse_index_iterator( First first,
		                                           difference_type pos )
		  : m_first( std::move( first ) )
		  , m_pos( pos ) {}

		[[nodiscard]] constexpr First base( ) const {
			return m_first + ( m_pos + 1 );
		}

		[[nodiscard]] constexpr decltype( auto ) operator*( ) const {
			return *( m_first + m_pos );
		}

		[[nodiscard]] constexpr auto operator->( ) const {
			return std::to_address( m_first + m_pos );
		}

		[[nodiscard]] constexpr decltype( auto )
		operator[]( difference_type n ) const {
			return *( m_first + ( m_pos - n ) );
		}

		constexpr reverse_index_iterator &operator++( ) noexcept {
			--m_pos;
			return *this;
		}

		[[nodiscard]] constexpr reverse_index_iterator operator++( int ) noexcept {
			auto tmp = *this;
			--m_pos;
			return tmp;
		}

		constexpr reverse_index_iterator &operator--( ) noexcept {
			++m_pos;
			return *this;
		}

		[[nodiscard]] constexpr reverse_index_iterator operator--( int ) noexcept {
			auto tmp = *this;
			++m_pos;
			return tmp;
		}

		constexpr reverse_index_iterator &operator+=( difference_type n ) {
			m_pos -= n;
			return *this;
		}

		constexpr reverse_index_iterator &operator-=( difference_type n ) {
			m_pos += n;
			return *this;
		}

		[[nodiscard]] constexpr reverse_index_iterator
		operator+( difference_type n ) const {
			auto result = *this;
			result += n;
			return result;
		}

		[[nodiscard]] constexpr reverse_index_iterator
		operator-( difference_type n ) const {
			auto result = *this;
			result -= n;
			return result;
		}

		[[nodiscard]] friend constexpr reverse_index_iterator
		operator+( difference_type n, reverse_index_iterator const &it ) {
			return it + n;
		}

		[[nodiscard]] friend constexpr difference_type
		operator-( reverse_index_iterator const &lhs,
		           reverse_index_iterator const &rhs ) {
			return rhs.m_pos - lhs.m_pos;
		}

		[[nodiscard]] friend constexpr bool
		operator==( reverse_index_iterator const &lhs,
		            reverse_index_iterator const &rhs ) {
			return lhs.m_pos == rhs.m_pos;
		}

		// clang-format off
		[[nodiscard]] friend constexpr auto
		operator<=>( reverse_index_iterator const &lhs,
		             reverse_index_iterator const &rhs ) {
			return rhs.m_pos <=> lhs.m_pos;
		}
		// clang-format on
	};

	template<typename It, typename Last>
	concept reverse_indexable = counted_source<It, Last>;
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<BidirectionalRange R>
	struct reverse_view : private maybe_owning_range<R> {
		using base_t = maybe_owning_range<R>;

	private:
		static constexpr bool counted_v =
		  pimpl::reverse_indexable<typename base_t::iterator,
		                           typename base_t::iterator_last>;
		static constexpr bool counted_const_v =
		  pimpl::reverse_indexable<typename base_t::const_iterator,
		                           typename base_t::const_iterator_last>;

	public:
		using iterator = std::conditional_t<
		  counted_v, pimpl::reverse_index_iterator<typename base_t::iterator>,
		  daw::reverse_iterator<typename base_t::iterator_last>>;
		using const_iterator = std::conditional_t<
		  counted_const_v,
		  pimpl::reverse_index_iterator<typename base_t::const_iterator>,
		  daw::reverse_iterator<typename base_t::const_iterator_last>>;
		using iterator_last = std::conditional_t<
		  counted_v, pimpl::reverse_index_iterator<typename base_t::iterator>,
		  daw::reverse_iterator<typename base_t::iterator>>;
		using const_iterator_last = std::conditional_t<
		  counted_const_v,
		  pimpl::reverse_index_iterator<typename base_t::const_iterator>,
		  daw::reverse_iterator<typename base_t::const_iterator>>;
		using daw_i_am_a_reverse_view_class = void;

		using base_t::is_owned;

		[[nodiscard]] constexpr auto &get_range( ) &
		requires( not std::is_const_v<std::remove_reference_t<R>> )
		{
			return static_cast<base_t *>( this )->get_range( );
		}

		[[nodiscard]] constexpr auto const &get_range( ) const & {
			return static_cast<base_t const *>( this )->get_range( );
		}

		reverse_view( ) = default;

		explicit constexpr reverse_view( daw::constructible<base_t> auto &&r )
		  : base_t{ DAW_FWD( r ) } {}

		[[nodiscard]] constexpr iterator begin( ) {
			if constexpr( counted_v ) {
				auto first = base_t::begin( );
				auto const len = pimpl::counted_length( first, base_t::end( ) );
				return iterator{ std::move( first ), len - 1 };
			} else {
				return iterator{ base_t::end( ) };
			}
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			if constexpr( counted_const_v ) {
				auto first = base_t::begin( );
				auto const len = pimpl::counted_length( first, base_t::end( ) );
				return const_iterator{ std::move( first ), len - 1 };
			} else {
				return const_iterator{ base_t::end( ) };
			}
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			if constexpr( counted_v ) {
				return iterator_last{ base_t::begin( ), -1 };
			} else {
				return iterator_last{ base_t::begin( ) };
			}
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			if constexpr( counted_const_v ) {
				return const_iterator_last{ base_t::begin( ), -1 };
			} else {
				return const_iterator_last{ base_t::begin( ) };
			}
		}

		[[nodiscard]] constexpr bool
		operator==( reverse_view const & ) const = default;
	};
	template<typename R>
	reverse_view( R && ) -> reverse_view<R>;
} // namespace daw::pipelines

namespace daw::pipelines::pimpl {
	struct Reverse_t {
		explicit Reverse_t( ) = default;

		template<BidirectionalRange R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr daw::remove_rvalue_ref_t<R>
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			auto first = std::begin( r );
			auto const last = std::end( r );
			auto it = safe_move_next( first, last );
			auto tail = it;

			if constexpr( RandomRange<R> ) {
				if( first != last ) {
					--tail;
					while( first < tail ) {
						std::iter_swap( first, tail );
						++first;
						--tail;
					}
				}
			} else {
				while( true ) {
					if( first == tail || first == --tail ) {
						break;
					}
					std::iter_swap( first, tail );
					++first;
				}
			}
			return DAW_FWD( r );
		}
	};

	struct ReverseView_t {
		explicit ReverseView_t( ) = default;

		template<BidirectionalRange R>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return reverse_view<R>{ DAW_FWD( r ) };
		}
	};
} // namespace daw::pipelines::pimpl
namespace daw::pipelines {
	inline constexpr auto Reverse = pimpl::Reverse_t{ };
	inline constexpr auto ReverseView = pimpl::ReverseView_t{ };
} // namespace daw::pipelines
