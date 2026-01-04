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
#include "range.h"

#include <cstddef>
#include <iterator>

namespace daw::pipelines {

	template<typename Iterator>
	struct unique_view {
		using value_type = daw::iter_value_t<Iterator>;
		static_assert( requires( value_type const &v ) { v == v; } );

		using iterator_category = std::forward_iterator_tag;
		using reference = daw::iter_reference_t<Iterator>;
		using const_reference = daw::iter_const_reference_t<Iterator>;
		using difference_type = std::ptrdiff_t;

	private:
		Iterator m_first = { };
		Iterator m_last = { };

	public:
		explicit unique_view( ) = default;

		explicit constexpr unique_view( Iterator first, Iterator last )
		  : m_first( first )
		  , m_last( last ) {}

		[[nodiscard]] constexpr unique_view begin( ) const {
			return *this;
		}

		[[nodiscard]] constexpr unique_view end( ) const {
			return unique_view( m_last, m_last );
		}

		[[nodiscard]] constexpr reference operator*( ) {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *m_first;
		}

		[[nodiscard]] constexpr auto operator->( ) const {
			return m_first.operator->( );
		}

		[[nodiscard]] constexpr auto operator->( ) {
			return m_first.operator->( );
		}

		[[nodiscard]] constexpr bool good( ) const {
			return m_first != m_last;
		}
		[[nodiscard]] explicit constexpr operator bool( ) const {
			return good( );
		}

		constexpr unique_view &operator++( ) {
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

		[[nodiscard]] constexpr unique_view operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] constexpr friend bool operator==( unique_view const &lhs,
		                                                unique_view const &rhs ) {
			return lhs.m_first == rhs.m_first;
		}

		[[nodiscard]] constexpr friend bool operator!=( unique_view const &lhs,
		                                                unique_view const &rhs ) {
			return lhs.m_first != rhs.m_first;
		}
	};
	template<typename I>
	unique_view( I ) -> unique_view<I>;

	template<Range R>
	struct unique_range {
		using value_type = daw::range_value_t<R>;
		static_assert( requires( value_type const &v ) { v == v; } );
		using iterator = unique_view<daw::iterator_t<R>>;

	private:
		R m_range;

	public:
		explicit constexpr unique_range( R const &r ) noexcept
		  : m_range{ r } {}

		explicit constexpr unique_range( R &&r ) noexcept
		  : m_range{ std::move( r ) } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ std::begin( m_range ), std::end( m_range ) };
		}

		[[nodiscard]] constexpr iterator begin( ) const {
			return iterator{ std::begin( m_range ), std::end( m_range ) };
		}

		[[nodiscard]] constexpr iterator end( ) {
			return iterator{ std::end( m_range ), std::end( m_range ) };
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return iterator{ std::end( m_range ), std::end( m_range ) };
		}

		[[nodiscard]] constexpr friend bool
		operator==( unique_range const &lhs, unique_range const &rhs ) = default;

		[[nodiscard]] constexpr friend bool
		operator!=( unique_range const &lhs, unique_range const &rhs ) = default;
	};

	template<Range R>
	unique_range( R ) -> unique_range<R>;

	namespace pimpl {
		struct Unique_t {
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( Range auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				if constexpr( std::is_rvalue_reference_v<decltype( r )> ) {
					return unique_range{ DAW_FWD( r ) };
				} else {
					return unique_view{ DAW_FWD( r ) };
				}
			}
		};
	} // namespace pimpl

	inline constexpr auto Unique = pimpl::Unique_t{ };
} // namespace daw::pipelines
