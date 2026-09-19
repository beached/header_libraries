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
#include "daw/pipelines/range_base.h"
#include "daw/pipelines/view.h"

#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <utility>

namespace daw::pipelines::pimpl {
	template<Iterator SentinelFor>
	struct filter_iterator_end {
		using iterator_category =
		  daw::common_iterator_category_t<std::forward_iterator_tag,
		                                  daw::iterator_category_t<SentinelFor>>;
		using difference_type = std::ptrdiff_t;
		using value_type = std::common_type_t<iter_value_t<SentinelFor>>;
		using reference = std::common_reference_t<iter_reference_t<SentinelFor>>;
		using pointer = void;
		using i_am_a_daw_filter_iterator_end_class = void;

		filter_iterator_end( ) = default;

		constexpr bool operator==( filter_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline filter_iterator_end &
		operator++( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline filter_iterator_end
		operator++( int ) const {
			std::terminate( );
		}
	};

	template<typename FI, Iterator First>
	struct filter_iterator {
		using iterator_category =
		  common_iterator_category_t<std::forward_iterator_tag,
		                             daw::iter_category_t<First>>;
		using value_type = daw::iter_value_t<First>;
		using reference = daw::iter_reference_t<First>;
		using const_reference = daw::iter_const_reference_t<First>;
		using pointer = daw::iter_pointer_t<First>;
		using const_pointer = std::remove_pointer_t<pointer> const *;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_filter_iterator_class = void;

	private:
		using fi_t = FI;
		using fi_pointer_t = FI *;
		using fi_const_pointer_t = FI const *;
		fi_pointer_t m_parent{ };
		First m_first{ };

		[[nodiscard]] constexpr bool filter( auto const &v ) {
			assert( m_parent );
			auto &parent = *m_parent;
			return std::invoke( parent.m_fn, std::invoke( parent.m_proj, v ) );
		}

		[[nodiscard]] constexpr bool filter( auto const &v ) const {
			assert( m_parent );
			auto const &parent = *m_parent;
			return std::invoke( parent.m_fn, std::invoke( parent.m_proj, v ) );
		}

	public:
		filter_iterator( ) = default;

		explicit constexpr filter_iterator( fi_pointer_t parent,
		                                    daw::constructible<First> auto &&first )
		  : m_parent{ parent }
		  , m_first( DAW_FWD( first ) ) {
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

		[[nodiscard]] constexpr reference operator*( ) {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return *m_first;
		}

		[[nodiscard]] constexpr pointer operator->( ) {
			return std::to_address( m_first );
		}

		[[nodiscard]] constexpr const_pointer operator->( ) const {
			return std::to_address( m_first );
		}

		[[nodiscard]] constexpr bool good( ) const {
			return m_first != m_parent->raw_end( );
		}

		[[nodiscard]] explicit constexpr operator bool( ) const {
			return good( );
		}

		constexpr filter_iterator &operator++( ) {
			++m_first;
			while( good( ) and not filter( *m_first ) ) {
				++m_first;
			}
			return *this;
		}

		[[nodiscard]] constexpr filter_iterator operator++( int ) {
			auto result = *this;
			operator++( );
			return result;
		}

		[[nodiscard]] constexpr bool
		operator==( filter_iterator const &rhs ) const {
			return m_first == rhs.m_first;
		}

		[[nodiscard]] constexpr bool
		operator==( filter_iterator_end<First> const & ) const {
			return not good( );
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<Range R, typename Fn, typename Projection = std::identity>
	struct filter_view
	  : private pimpl::stored_range_base_t<
	      R, //
	      pimpl::filter_iterator<filter_view<R, Fn, Projection>, iterator_t<R>>,
	      pimpl::filter_iterator_end<iterator_t<R>>,
	      pimpl::filter_iterator<filter_view<R, Fn, Projection> const,
	                             const_iterator_t<R>>,
	      pimpl::filter_iterator_end<const_iterator_t<R>>> {

		using base_t = pimpl::stored_range_base_t<
		  R, //
		  pimpl::filter_iterator<filter_view, iterator_t<R>>,
		  pimpl::filter_iterator_end<iterator_t<R>>,
		  pimpl::filter_iterator<filter_view const, const_iterator_t<R>>,
		  pimpl::filter_iterator_end<const_iterator_t<R>>>;

		using i_am_a_daw_filter_view_class = void;

		using iterator = typename base_t::iterator_first_t;
		using const_iterator = typename base_t::const_iterator_first_t;
		using iterator_last = typename base_t::iterator_last_t;
		using const_iterator_last = typename base_t::const_iterator_last_t;

	private:
		template<typename, Iterator>
		friend struct pimpl::filter_iterator;

		[[nodiscard]] constexpr auto raw_end( ) {
			return base_t::rend( );
		}

		[[nodiscard]] constexpr auto raw_end( ) const {
			return base_t::rend( );
		}

		DAW_NO_UNIQUE_ADDRESS Fn m_fn;
		DAW_NO_UNIQUE_ADDRESS Projection m_proj{ };

	public:
		explicit filter_view( ) = default;

		template<Range R0, typename F>
		requires(
		  std::constructible_from<base_t, R0> and std::constructible_from<Fn, F> and
		  not Iterator<F> ) //
		  explicit constexpr filter_view( R0 &&r, F &&fn )
		  : base_t( DAW_FWD( r ) )
		  , m_fn{ DAW_FWD( fn ) } {}

		template<Range R0, typename F, typename P>
		requires(
		  std::constructible_from<base_t, R0> and std::constructible_from<Fn, F> and
		  not Iterator<F> and std::constructible_from<Projection, P> and
		  not Iterator<P> ) //
		  explicit constexpr filter_view( R0 &&r, F &&fn, P &&projection )
		  : base_t( DAW_FWD( r ) )
		  , m_fn{ DAW_FWD( fn ) }
		  , m_proj( DAW_FWD( projection ) ) {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator( this, base_t::rbegin( ) );
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator( this, base_t::rbegin( ) );
		}

		[[nodiscard]] constexpr iterator_last end( ) {
			return iterator_last{ };
		}

		[[nodiscard]] constexpr const_iterator_last end( ) const {
			return const_iterator_last{ };
		}

		constexpr bool operator==( filter_view const & ) const = default;
	};
	template<Range R, typename F>
	requires( not Iterator<F> ) //
	  filter_view( R &&, F ) -> filter_view<R, F>;

	template<Range R, typename F, typename P>
	requires( not Iterator<F> and not Iterator<P> ) //
	  filter_view( R &&, F, P ) -> filter_view<daw::remove_rvalue_ref_t<R>, F, P>;

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
				return filter_view<R, Fn>( DAW_FWD( r ), m_func, m_projection );
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
