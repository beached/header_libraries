// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_is_constant_evaluated.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_print.h"
#include "daw/daw_remove_cvref.h"
#include "daw/daw_typeof.h"
#include "daw/pipelines/range.h"
#include "daw/pipelines/range_base.h"

#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <utility>

namespace daw::pipelines {
	template<typename Iterator, typename Filter,
	         typename Projection = std::identity>
	struct filter_view : range_base_t<Iterator> {
		using daw_range_base_t = range_base_t<Iterator>;
		using typename daw_range_base_t::iterator_first_t;
		using typename daw_range_base_t::iterator_last_t;

		using iterator_category = std::forward_iterator_tag;
		using value_type = daw::iter_value_t<iterator_first_t>;
		using reference = daw::iter_reference_t<iterator_first_t>;
		using const_reference = daw::iter_const_reference_t<iterator_first_t>;
		using pointer = daw::iter_pointer_t<iterator_first_t>;
		using difference_type = std::ptrdiff_t;

	private:
		iterator_first_t m_first{ };
		iterator_last_t m_last{ };
		DAW_NO_UNIQUE_ADDRESS mutable Filter m_func{ };
		DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		filter( auto const &v ) const {
			return std::invoke( m_func, m_projection( v ) );
		}

	public:
		explicit filter_view( ) = default;

		explicit constexpr filter_view(
		  iterator_first_t first, iterator_last_t last, Filter const &fn,
		  Projection const &projection = Projection{ } )
		  : m_first( first )
		  , m_last( last )
		  , m_func( fn )
		  , m_projection( projection ) {
			while( good( ) and not filter( *m_first ) ) {
				++m_first;
			}
		}

		[[nodiscard]] constexpr auto &base( ) {
			return m_first;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr filter_view begin( ) const {
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr filter_view end( ) const {
			return filter_view( m_last, m_last, m_func, m_projection );
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
			++m_first;
			while( good( ) and not filter( *m_first ) ) {
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

	namespace pimpl {
		template<typename Fn, typename Projection = std::identity>
		struct filter_t {
			DAW_NO_UNIQUE_ADDRESS mutable Fn m_func;
			DAW_NO_UNIQUE_ADDRESS Projection m_projection{ };

			template<Range R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				static_assert( std::invocable<Projection, range_value_t<R>>,
				               "Projection must be invocable with range_value_t<R>" );
				using projected_t = std::invoke_result_t<Projection, range_value_t<R>>;
				static_assert(
				  std::invocable<Fn, projected_t>,
				  "Filter requires an invokable function with the projected value" );
				static_assert(
				  std::convertible_to<std::invoke_result_t<Fn, projected_t>, bool>,
				  "Filter requires an invokable function that returns a bool" );
				return filter_view<iterator_t<R>, Fn>( std::begin( r ), std::end( r ),
				                                       m_func, m_projection );
			}

			template<typename Value>
			[[nodiscard]] constexpr auto operator( )( Value &&v ) const {
				static_assert( std::invocable<Projection, Value>,
				               "Projection must be invocable with range_value_t<R>" );
				using projected_t = std::invoke_result_t<Projection, Value>;
				static_assert(
				  std::is_invocable_r_v<bool, Fn, projected_t>,
				  "Filter requires an invokable function that returns a bool" );
				using result_t = daw::remove_cvref_t<std::invoke_result_t<Fn, Value>>;
				if( std::invoke( m_func, std::invoke( m_projection, v ) ) ) {
					return std::optional<result_t>( std::invoke( m_func, DAW_FWD( v ) ) );
				}
				return std::optional<result_t>( );
			}
		};

		template<typename F>
		filter_t( F ) -> filter_t<F>;

		template<typename F, typename P>
		filter_t( F, P ) -> filter_t<F, P>;
	} // namespace pimpl

	/// Filter a range with the given predicate function.
	template<typename Fn>
	[[nodiscard]] constexpr auto Filter( Fn &&fn ) {
		return pimpl::filter_t{ DAW_FWD( fn ) };
	};
} // namespace daw::pipelines
