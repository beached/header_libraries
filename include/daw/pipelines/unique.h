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
#include "view.h"

#include <cstddef>
#include <iterator>

namespace daw::pipelines {
	template<Iterator SentinelFor>
	struct unique_iterator_end {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::common_type_t<iter_value_t<SentinelFor>>;
		using reference = std::common_reference_t<iter_reference_t<SentinelFor>>;
		using pointer = void;
		using i_am_a_daw_unique_iterator_end_class = void;

		unique_iterator_end( ) = default;

		constexpr bool operator==( unique_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline unique_iterator_end &
		operator++( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline unique_iterator_end
		operator++( int ) const {
			std::terminate( );
		}
	};

	template<ForwardIterator First, typename Last>
	struct unique_iterator {
		using value_type = daw::iter_value_t<First>;
		static_assert( requires( value_type const &v ) { v == v; } );

		using iterator_category = std::forward_iterator_tag;
		using reference = daw::iter_reference_t<First>;
		using const_reference = daw::iter_const_reference_t<First>;
		using difference_type = std::ptrdiff_t;

	private:
		First m_first{ };
		Last m_last{ };

	public:
		unique_iterator( ) = default;

		template<ForwardIterator F, typename L>
		requires( std::constructible_from<First, F>
		            and std::constructible_from<Last, L> ) //
		  explicit constexpr unique_iterator( F &&first, L &&last )
		  : m_first( DAW_FWD( first ) )
		  , m_last( DAW_FWD( last ) ) {}

		[[nodiscard]] constexpr reference operator*( ) {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *m_first;
		}

		[[nodiscard]] constexpr auto operator->( ) const {
			return std::to_address( m_first );
		}

		[[nodiscard]] constexpr auto operator->( ) {
			return std::to_address( m_first );
		}

		[[nodiscard]] constexpr bool good( ) const {
			return m_first != m_last;
		}
		[[nodiscard]] explicit constexpr operator bool( ) const {
			return good( );
		}

		constexpr unique_iterator &operator++( ) {
			if( not good( ) ) {
				return *this;
			}
			auto const v = *m_first;
			++m_first;
			while( good( ) and *m_first == v ) {
				++m_first;
			}
			return *this;
		}

		[[nodiscard]] constexpr unique_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] constexpr bool
		operator==( unique_iterator const &rhs ) const {
			return m_first == rhs.m_first;
		}

		[[nodiscard]] constexpr bool
		operator==( unique_iterator_end<Last> const & ) const {
			return not good( );
		}
	};
	template<ForwardRange R>
	struct unique_view
	  : private pimpl::stored_range_base_t<
	      R, unique_iterator<iterator_t<R>, iterator_end_t<R>>,
	      unique_iterator_end<iterator_end_t<R>>,
	      unique_iterator<const_iterator_t<R>, const_iterator_end_t<R>>,
	      unique_iterator_end<const_iterator_end_t<R>>> {

		using base_t = pimpl::stored_range_base_t<
		  R, unique_iterator<iterator_t<R>, iterator_end_t<R>>,
		  unique_iterator_end<iterator_end_t<R>>,
		  unique_iterator<const_iterator_t<R>, const_iterator_end_t<R>>,
		  unique_iterator_end<const_iterator_end_t<R>>>;

		using iterator = typename base_t::iterator_first_t;
		using iterator_last = typename base_t::iterator_last_t;
		using const_iterator = typename base_t::const_iterator_first_t;
		using const_iterator_last = typename base_t::const_iterator_last_t;

		explicit unique_view( ) = default;

		template<ForwardRange FR>
		requires( std::constructible_from<R, FR> ) //
		  explicit constexpr unique_view( FR &&range )
		  : base_t( DAW_FWD( range ) ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ base_t::rbegin( ), base_t::rend( ) };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{ base_t::rbegin( ), base_t::rend( ) };
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return iterator_last{ };
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return const_iterator_last{ };
		}

		[[nodiscard]] constexpr bool
		operator==( unique_view const & ) const = default;
	};
	template<ForwardRange R>
	unique_view( R && ) -> unique_view<R>;

	namespace pimpl {
		struct Unique_t {
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( ForwardRange auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return unique_view{ DAW_FWD( r ) };
			}
		};
	} // namespace pimpl

	inline constexpr auto Unique = pimpl::Unique_t{ };
} // namespace daw::pipelines
