// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_mutable_function_ref.h"
#include "daw/daw_tuple_forward.h"
#include "daw/daw_typeof.h"
#include "daw/iterator/daw_arrow_proxy.h"
#include "daw/pipelines/pipeline_traits.h"
#include "daw/pipelines/range_base.h"

#include <cstddef>
#include <functional>
#include <iterator>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace daw::pipelines {
	template<Range R, typename Fn, typename Projection = std::identity>
	struct map_view;
}

namespace daw::pipelines::pimpl {
	template<typename Fn, typename... Args>
	consteval bool is_const_fn_same_v( ) {
		if constexpr( std::is_invocable_v<Fn const, Args...> ) {
			using fn_result_t = std::remove_reference_t<
			  std::remove_const_t<std::invoke_result_t<Fn, Args...>>>;
			using fn_c_result_t = std::remove_reference_t<
			  std::remove_const_t<std::invoke_result_t<Fn const, Args...>>>;
			return std::is_convertible_v<fn_c_result_t, fn_result_t>;
		} else {
			return true;
		}
	}

	template<Iterator SentinelFor>
	struct map_iterator_end {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::common_type_t<iter_value_t<SentinelFor>>;
		using reference = std::common_reference_t<iter_reference_t<SentinelFor>>;
		using pointer = void;
		using i_am_a_daw_map_iterator_end_class = void;

		map_iterator_end( ) = default;

		constexpr bool operator==( map_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline map_iterator_end &
		operator++( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE inline map_iterator_end
		operator++( int ) const {
			std::terminate( );
		}
	};

	template<typename MI, Iterator I, Iterator ILast, typename Fn,
	         typename Projection>
	struct map_iterator {
		using iterator_category = daw::iter_category_t<I>;

	private:
		using iterator = I;
		using last_iterator = map_iterator_end<ILast>;
		using iter_ref_t =
		  daw::remove_rvalue_ref_t<daw::iter_reference_t<iterator>>;
		using proj_result_t =
		  daw::remove_rvalue_ref_t<std::invoke_result_t<Projection, iter_ref_t>>;
		using func_result_t =
		  daw::remove_rvalue_ref_t<std::invoke_result_t<Fn, proj_result_t>>;

		static_assert( pimpl::is_const_fn_same_v<Projection, iter_ref_t>( ),
		               "Expect similar results for const/non-const Projection" );
		static_assert( pimpl::is_const_fn_same_v<Fn, proj_result_t>( ),
		               "Expect similar results for const/non-const Fn" );

	public:
		using reference = func_result_t;
		using value_type = std::remove_cvref_t<reference>;
		using const_reference = reference;
		using pointer = arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

	private:
		MI *m_parent{ };
		iterator m_iter{ };

	public:
		map_iterator( ) = default;

		template<Iterator First>
		requires( std::constructible_from<iterator, First> ) //
		  explicit constexpr map_iterator( MI *parent, First &&first )
		  : m_parent( parent )
		  , m_iter( DAW_FWD( first ) ) {}

	private:
		[[nodiscard]] constexpr decltype( auto ) raw_get( size_type n )
		  requires( RandomIterator<iterator> ) {
			return *( m_iter + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] constexpr decltype( auto ) raw_get( size_type n ) const
		  requires( RandomIterator<iterator> ) {
			return *( m_iter + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] constexpr decltype( auto ) do_project( auto &&v ) {
			return std::invoke( m_parent->m_proj, DAW_FWD( v ) );
		}

		[[nodiscard]] constexpr decltype( auto ) do_project( auto &&v ) const {
			return std::invoke( static_cast<MI const *>( m_parent )->m_proj,
			                    DAW_FWD( v ) );
		}

		[[nodiscard]] constexpr decltype( auto ) do_func( auto &&v ) {
			return std::invoke( m_parent->m_fn, do_project( DAW_FWD( v ) ) );
		}

		[[nodiscard]] constexpr decltype( auto ) do_func( auto &&v ) const {
			return std::invoke( static_cast<MI const *>( m_parent )->m_fn,
			                    do_project( DAW_FWD( v ) ) );
		}

	public:
		[[nodiscard]] constexpr auto &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_iter;
		}

		[[nodiscard]] constexpr bool good( ) const {
			return m_iter != m_parent->rend( );
		}

		[[nodiscard]] constexpr reference operator[]( size_type n )
		  requires( RandomIterator<iterator> ) {
			return do_func( raw_get( n ) );
		}

		[[nodiscard]] constexpr reference operator[]( size_type n ) const
		  requires( RandomIterator<iterator> ) {
			return do_func( raw_get( n ) );
		}

		[[nodiscard]] constexpr reference operator*( ) {
			return do_func( *m_iter );
		}

		[[nodiscard]] constexpr reference operator*( ) const {
			return do_func( *m_iter );
		}

		[[nodiscard]] constexpr pointer operator->( ) {
			return do_func( *m_iter );
		}

		[[nodiscard]] constexpr pointer operator->( ) const {
			return do_func( *m_iter );
		}

		constexpr map_iterator &operator++( ) {
			++m_iter;
			return *this;
		}

		[[nodiscard]] constexpr map_iterator operator++( int ) {
			map_iterator result = *this;
			++m_iter;
			return result;
		}

		constexpr map_iterator &operator--( )
		  requires( BidirectionalIterator<iterator> ) {
			--m_iter;
			return *this;
		}

		[[nodiscard]] constexpr map_iterator operator--( int )
		  requires( BidirectionalIterator<iterator> ) {
			map_iterator result = *this;
			--m_iter;
			return result;
		}

		constexpr map_iterator &operator+=( difference_type n )
		  requires( RandomIterator<iterator> ) {
			m_iter += n;
			return *this;
		}

		constexpr map_iterator &operator-=( difference_type n )
		  requires( RandomIterator<iterator> ) {
			m_iter -= n;
			return *this;
		}

		[[nodiscard]] friend constexpr map_iterator
		operator+( map_iterator lhs, difference_type n ) noexcept
		  requires( RandomIterator<iterator> ) {
			lhs += n;
			return lhs;
		}

		[[nodiscard]] friend constexpr map_iterator
		operator+( difference_type n, map_iterator rhs ) noexcept
		  requires( RandomIterator<iterator> ) {
			rhs += n;
			return rhs;
		}

		[[nodiscard]] friend constexpr map_iterator
		operator-( map_iterator lhs, difference_type n ) noexcept
		  requires( RandomIterator<iterator> ) {
			lhs -= n;
			return lhs;
		}

		[[nodiscard]] friend constexpr map_iterator
		operator-( difference_type n, map_iterator rhs ) noexcept
		  requires( RandomIterator<iterator> ) {
			rhs -= n;
			return rhs;
		}

		[[nodiscard]] constexpr difference_type
		operator-( map_iterator const &rhs ) const
		  requires( RandomIterator<iterator> ) {
			return m_iter - rhs.m_iter;
		}

		[[nodiscard]] constexpr difference_type
		operator-( last_iterator const & ) const
		  requires( RandomIterator<iterator> ) {
			return m_iter - m_parent->rend( );
		}

		[[nodiscard]] friend constexpr difference_type
		operator-( last_iterator const &, map_iterator const &rhs )
		  requires( RandomIterator<iterator> ) {
			return rhs.m_parent->rend( ) - rhs.m_iter;
		}

		[[nodiscard]] constexpr bool operator==( map_iterator const &rhs ) const {
			return m_iter == rhs.m_iter;
		}

		[[nodiscard]] constexpr bool operator==( last_iterator const & ) const {
			return not good( );
		}

		// clang-format off
		[[nodiscard]] constexpr auto operator<=>( map_iterator const &rhs )
			requires( RandomIterator<iterator> ) {
			return m_iter <=> rhs.m_iter;
			}
		// clang-format on
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<Range R, typename Fn, typename Projection>
	struct map_view
	  : private pimpl::stored_range_base_t<
	      R, //
	      pimpl::map_iterator<map_view<R, Fn, Projection>, iterator_t<R>,
	                          iterator_end_t<R>, Fn, Projection>,
	      pimpl::map_iterator_end<iterator_end_t<R>>,
	      pimpl::map_iterator<map_view<R, Fn, Projection> const,
	                          const_iterator_t<R>, const_iterator_end_t<R>, Fn,
	                          Projection>,
	      pimpl::map_iterator_end<const_iterator_end_t<R>>> {

		using base_t = pimpl::stored_range_base_t<
		  R, //
		  pimpl::map_iterator<map_view<R, Fn, Projection>, iterator_t<R>,
		                      iterator_end_t<R>, Fn, Projection>,
		  pimpl::map_iterator_end<iterator_end_t<R>>,
		  pimpl::map_iterator<map_view<R, Fn, Projection> const,
		                      const_iterator_t<R>, const_iterator_end_t<R>, Fn,
		                      Projection>,
		  pimpl::map_iterator_end<const_iterator_end_t<R>>>;

		using typename base_t::const_iterator_first_t;
		using typename base_t::const_iterator_last_t;
		using typename base_t::iterator_first_t;
		using typename base_t::iterator_last_t;

		using value_type = daw::iter_value_t<iterator_first_t>;

	private:
		template<typename, Iterator, Iterator, typename, typename>
		friend struct pimpl::map_iterator;

		DAW_NO_UNIQUE_ADDRESS Fn m_fn;
		DAW_NO_UNIQUE_ADDRESS Projection m_proj{ };

	public:
		explicit map_view( ) = default;

		template<Range R0, typename F>
		requires(
		  std::constructible_from<base_t, R0> and std::constructible_from<Fn, F> and
		  not Iterator<F> ) //
		  explicit constexpr map_view( R0 &&r, F &&fn )
		  : base_t( DAW_FWD( r ) )
		  , m_fn{ DAW_FWD( fn ) } {}

		template<Range R0, typename F, typename P>
		requires(
		  std::constructible_from<base_t, R0> and std::constructible_from<Fn, F> and
		  not Iterator<F> and std::constructible_from<Projection, P> and
		  not Iterator<P> ) //
		  explicit constexpr map_view( R0 &&r, F &&fn, P &&projection )
		  : base_t( DAW_FWD( r ) )
		  , m_fn{ DAW_FWD( fn ) }
		  , m_proj( DAW_FWD( projection ) ) {}

		[[nodiscard]] constexpr iterator_first_t begin( ) {
			return iterator_first_t( this, base_t::rbegin( ) );
		}

		[[nodiscard]] constexpr const_iterator_first_t begin( ) const {
			return const_iterator_first_t( this, base_t::rbegin( ) );
		}

		[[nodiscard]] constexpr iterator_last_t end( ) {
			return iterator_last_t{ };
		}

		[[nodiscard]] constexpr const_iterator_last_t end( ) const {
			return const_iterator_last_t{ };
		}
	};
	template<Range R, typename F>
	requires( not Iterator<F> ) //
	  map_view( R &&, F ) -> map_view<R, F>;

	template<Range R, typename F, typename P>
	requires( not Iterator<F> and not Iterator<P> ) //
	  map_view( R &&, F, P ) -> map_view<R, F, P>;

	namespace pimpl {
		template<typename Fn, typename Projection = std::identity>
		struct Map_t {
			DAW_NO_UNIQUE_ADDRESS Fn m_func;
			DAW_NO_UNIQUE_ADDRESS Projection m_projection = Projection{ };

			[[nodiscard]] constexpr auto operator( )( auto &&r ) const {
				using R = decltype( r );
				if constexpr( Range<R> ) {
					static_assert(
					  std::invocable<Projection, range_reference_t<R>>,
					  "Projection must be invocable with the range_value_t<R>" );
					using projected_t =
					  std::invoke_result_t<Projection, range_reference_t<R>>;

					static_assert( std::invocable<Fn, projected_t>,
					               "Map requires the function to be able to be called "
					               "with invoke and the range_reference_t(e.g. invoke( "
					               "MapFn, *it ) )" );
					static_assert( traits::NoVoidResults<Fn, projected_t>,
					               "Map requires the result to not be void" );
					return map_view( DAW_FWD( r ), m_func, m_projection );
				} else {
					static_assert( std::invocable<Projection, R>,
					               "Projection must be invocable with R" );
					using projected_t = std::invoke_result_t<Projection, R>;

					static_assert( std::invocable<Fn, projected_t>,
					               "Map requires the function to be able to be called "
					               "with invoke and passed value" );
					static_assert( traits::NoVoidResults<Fn, projected_t>,
					               "Map requires the result to not be void" );
					return std::invoke( m_func,
					                    std::invoke( m_projection, DAW_FWD( r ) ) );
				}
			}
		};
		template<typename Fn>
		Map_t( Fn ) -> Map_t<Fn>;

		template<typename Fn, typename Projection>
		Map_t( Fn, Projection ) -> Map_t<Fn, Projection>;

		template<typename Fn>
		struct MapApply_t {
			DAW_NO_UNIQUE_ADDRESS Fn m_func;

			template<Range R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				static_assert( traits::is_applicable_v<Fn, range_reference_t<R>>,
				               "MapApply requires the function to be able to be called "
				               "with apply and the range_reference_t" );
				auto func = m_func;
				return map_view{ DAW_FWD( r ), [=]( auto &&tp ) {
					                return std::apply( func, tp );
				                } };
			}
		};
		template<typename Fn>
		MapApply_t( Fn ) -> MapApply_t<Fn>;

		template<typename T, typename Compare>
		struct Clamp_t {
			T lo;
			T hi;
			DAW_NO_UNIQUE_ADDRESS Compare compare;

			template<typename R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				auto h = hi;
				auto l = lo;
				auto c = compare;
				if constexpr( Range<R> ) {
					using value_type = range_value_t<R>;
					static_assert( std::convertible_to<T, value_type>,
					               "Clamp requires a lo/hi values convertible to the "
					               "range value type" );
					return map_view{ DAW_FWD( r ), [=]( value_type const &v ) {
						                return std::clamp( v, l, h, c );
					                } };
				} else {
					static_assert(
					  std::convertible_to<T, daw::remove_cvref_t<R>>,
					  "Clamp requires a lo/hi values convertible to the value type" );
					return std::clamp( r, l, h, c );
				}
			}
		};
		template<typename T, typename Compare>
		Clamp_t( T, T, Compare ) -> Clamp_t<T, Compare>;
	} // namespace pimpl

	template<typename Fn, typename Projection = std::identity>
	[[nodiscard]] constexpr auto Map( Fn &&fn,
	                                  Projection &&projection = Projection( ) ) {
		return pimpl::Map_t<daw::remove_rvalue_ref_t<Fn>,
		                    daw::remove_rvalue_ref_t<Projection>>{
		  DAW_FWD( fn ), DAW_FWD( projection ) };
	};

	template<typename Fn>
	[[nodiscard]] constexpr auto MapApply( Fn &&fn ) {
		return pimpl::MapApply_t<daw::remove_rvalue_ref_t<Fn>>{ DAW_FWD( fn ) };
	}

	template<typename T, typename Compare = std::less<>>
	[[nodiscard]] constexpr auto Clamp( T &&lo, std::type_identity_t<T> hi,
	                                    Compare &&compare = Compare{ } ) {
		return pimpl::Clamp_t<daw::remove_rvalue_ref_t<T>,
		                      daw::remove_rvalue_ref_t<Compare>>{
		  DAW_FWD( lo ), DAW_FWD2( hi ), DAW_FWD( compare ) };
	}
} // namespace daw::pipelines
