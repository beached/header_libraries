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
#include "daw/daw_remove_cvref.h"
#include "daw/daw_typeof.h"
#include "range.h"

#include <cstddef>
#include <iterator>
#include <optional>
#include <utility>

namespace daw::pipelines {
	template<typename Iterator, typename Filter>
	struct filter_view {
		using iterator_category = std::forward_iterator_tag;
		using value_type = daw::iter_value_t<Iterator>;
		using reference = daw::iter_reference_t<Iterator>;
		using const_reference = daw::iter_const_reference_t<Iterator>;
		using pointer = daw::iter_pointer_t<Iterator>;
		using difference_type = std::ptrdiff_t;

	private:
		Iterator m_first = Iterator{ };
		Iterator m_last = Iterator{ };
		[[no_unique_address]] Filter filt = Filter( );

	public:
		explicit filter_view( ) = default;

		explicit constexpr filter_view( Iterator first, Iterator last, Filter fn )
		  : m_first( first )
		  , m_last( last )
		  , filt( std::move( fn ) ) {}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr filter_view begin( ) const {
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr filter_view end( ) const {
			return filter_view( m_last, m_last, filt );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) {
			return *m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator*( ) const {
			return *m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer operator->( ) const {
			return m_first.operator->( );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool good( ) const {
			return m_first != m_last;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE explicit constexpr operator bool( ) const {
			return good( );
		}

		DAW_ATTRIB_INLINE constexpr filter_view &operator++( ) {
			if( not good( ) ) {
				return *this;
			}
			++m_first;
			while( good( ) and not filt( *m_first ) ) {
				++m_first;
			}
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr filter_view operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator==( filter_view const &lhs, filter_view const &rhs ) {
			return lhs.m_first == rhs.m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator!=( filter_view const &lhs, filter_view const &rhs ) {
			return lhs.m_first != rhs.m_first;
		}
	};
	template<typename I, typename F>
	filter_view( I, I, F ) -> filter_view<I, F>;

	namespace pimple {
		template<typename Fn>
		struct filter_t {
			[[no_unique_address]] Fn fn;

			template<Range R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				static_assert(
				  std::is_invocable_v<Fn, range_reference_t<R>>,
				  "Filter requires an invokable function with range_reference_t" );
				static_assert(
				  std::convertible_to<std::invoke_result_t<Fn, range_reference_t<R>>,
				                      bool>,
				  "Filter requires an invokable function that returns a bool" );
				return filter_view<iterator_t<R>, Fn>(
				  std::begin( r ), std::end( r ), fn );
			}

			template<typename Value>
			[[nodiscard]] constexpr auto operator( )( Value &&v ) const {
				static_assert(
				  std::is_invocable_r_v<bool, Fn, Value>,
				  "Filter requires an invokable function that returns a bool" );
				using result_t = daw::remove_cvref_t<std::invoke_result_t<Fn, Value>>;
				if( fn( v ) ) {
					return std::optional<result_t>( DAW_FWD( v ) );
				}
				return std::optional<result_t>( );
			}
		};
		template<typename F>
		filter_t( F ) -> filter_t<F>;
	} // namespace pimple

	template<typename Fn>
	[[nodiscard]] constexpr auto Filter( Fn &&fn ) {
		return pimple::filter_t{ DAW_FWD( fn ) };
	};
} // namespace daw::pipelines
