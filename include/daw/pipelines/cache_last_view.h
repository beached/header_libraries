// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/iterator/daw_arrow_proxy.h"
#include "daw/pipelines/range.h"

#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>

namespace daw::pipelines {
	template<Iterator It>
	struct cache_last_iterator {
		using difference_type = daw::iter_difference_t<It>;
		using value_type = std::remove_reference_t<daw::iter_value_t<It>>;
		using reference = value_type &;
		using pointer = daw::arrow_proxy<value_type>;
		using iterator_category =
		  daw::common_iterator_category_t<daw::iter_category_t<It>,
		                                  std::bidirectional_iterator_tag>;

	private:
		It m_iterator = It{ };
		std::optional<value_type> m_cached = std::nullopt;

	public:
		cache_last_iterator( ) = default;

		explicit constexpr cache_last_iterator( It i )
		  : m_iterator( std::move( i ) ) {}

		constexpr reference value( ) {
			if( not m_cached ) {
				m_cached.emplace( *m_iterator );
			}
			return *m_cached;
		}

		constexpr reference operator*( ) {
			return value( );
		}

		constexpr pointer operator->( ) {
			return pointer{ value( ) };
		}

		constexpr cache_last_iterator &operator++( ) {
			m_cached.reset( );
			++m_iterator;
			return *this;
		}

		constexpr cache_last_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr cache_last_iterator &operator--( ) requires(
		  std::is_same_v<iterator_category, std::bidirectional_iterator_tag> ) {

			m_cached.reset( );
			--m_iterator;
			return *this;
		}

		constexpr cache_last_iterator operator--( int ) requires(
		  std::is_same_v<iterator_category, std::bidirectional_iterator_tag> ) {

			auto result = *this;
			operator--( );
			return result;
		}
		constexpr bool operator==( cache_last_iterator const &rhs ) const {
			return m_iterator == rhs.m_iterator;
		}

		constexpr bool operator!=( cache_last_iterator const &rhs ) const {
			return m_iterator != rhs.m_iterator;
		}

		constexpr bool operator==( It const &rhs ) const {
			return m_iterator == rhs;
		}

		constexpr bool operator!=( It const &rhs ) const {
			return m_iterator != rhs;
		}
	};

	template<Range R>
	struct cache_last_view {
		using iterator_first_t = cache_last_iterator<daw::iterator_t<R>>;
		using iterator_last_t = cache_last_iterator<daw::iterator_end_t<R>>;

	private:
		iterator_first_t m_first = iterator_first_t{ };
		iterator_last_t m_last = iterator_last_t{ };

	public:
		cache_last_view( ) = default;

		constexpr cache_last_view( std::convertible_to<R> auto &&r )
		  : m_first( std::begin( r ) )
		  , m_last( std::end( r ) ) {}

		constexpr iterator_first_t begin( ) {
			return m_first;
		}

		constexpr iterator_first_t begin( ) const {
			return m_first;
		}

		constexpr iterator_last_t end( ) {
			return m_last;
		}

		constexpr iterator_last_t end( ) const {
			return m_last;
		}
	};

	template<Range R>
	cache_last_view( R ) -> cache_last_view<R>;

	namespace pimpl {
		struct CacheLast_t {
			explicit CacheLast_t( ) = default;

			template<Range R>
			DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return cache_last_view{ DAW_FWD( r ) };
			}
		};
	} // namespace pimpl

	inline constexpr auto CacheLast = pimpl::CacheLast_t{ };
} // namespace daw::pipelines
