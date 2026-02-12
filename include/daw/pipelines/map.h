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
#include "daw/daw_tuple_forward.h"
#include "daw/daw_typeof.h"
#include "daw/iterator/daw_arrow_proxy.h"
#include "daw/pipelines/pipeline_traits.h"
#include "daw/pipelines/range_base.h"

#include <cstddef>
#include <functional>
#include <iterator>
#include <tuple>
#include <type_traits>

namespace daw::pipelines {
	template<Iterator Iterator, typename Fn, typename Projection = std::identity>
	struct map_iterator {
		using iterator_category = daw::iter_category_t<Iterator>;
		using value_type = daw::remove_cvref_t<std::invoke_result_t<
		  Fn, std::invoke_result_t<Projection, daw::iter_reference_t<Iterator>>>>;
		using reference = value_type;
		using const_reference = value_type;
		using pointer = arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

	private:
		Iterator m_iter{ };
		DAW_NO_UNIQUE_ADDRESS Fn m_func = Fn{ };
		DAW_NO_UNIQUE_ADDRESS Projection m_projection = Projection{ };

	public:
		explicit constexpr map_iterator( ) = default;
		explicit constexpr map_iterator( Iterator it, Fn f )
		  : m_iter( it )
		  , m_func( f ) {}

		explicit constexpr map_iterator( Iterator it, Fn f, Projection projection )
		  : m_iter( it )
		  , m_func( f )
		  , m_projection( projection ) {}

	private:
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr decltype( auto )
		raw_get( size_type n ) requires( RandomIterator<Iterator> ) {
			return *( m_iter + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr decltype( auto )
		raw_get( size_type n ) const requires( RandomIterator<Iterator> ) {
			return *( m_iter + static_cast<difference_type>( n ) );
		}

	public:
		[[nodiscard]] constexpr auto &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr value_type
		operator[]( size_type n ) const requires( RandomIterator<Iterator> ) {
			return std::invoke( m_func, raw_get( n ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr value_type operator*( ) {
			return std::invoke( m_func, *m_iter );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr value_type operator*( ) const {
			return std::invoke( m_func, *m_iter );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer operator->( ) {
			return pointer( std::invoke( m_func, *m_iter ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer operator->( ) const {
			return pointer( std::invoke( m_func, *m_iter ) );
		}

		DAW_ATTRIB_INLINE constexpr map_iterator &operator++( ) {
			++m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr map_iterator operator++( int ) {
			map_iterator result = *this;
			++m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr map_iterator &operator--( )
		  requires( BidirectionalIterator<Iterator> ) {
			--m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr map_iterator operator--( int )
		  requires( BidirectionalIterator<Iterator> ) {
			map_iterator result = *this;
			--m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr map_iterator &operator+=( difference_type n )
		  requires( RandomIterator<Iterator> ) {
			m_iter += n;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr map_iterator &operator-=( difference_type n )
		  requires( RandomIterator<Iterator> ) {
			m_iter -= n;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr map_iterator
		operator+( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			map_iterator result = *this;
			m_iter += n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr map_iterator
		operator-( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			map_iterator result = *this;
			m_iter -= n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( map_iterator const &rhs ) requires( RandomIterator<Iterator> ) {
			return m_iter - rhs.m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator==( map_iterator const &lhs, map_iterator const &rhs ) {
			return lhs.m_iter == rhs.m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator!=( map_iterator const &lhs, map_iterator const &rhs ) {
			return lhs.m_iter != rhs.m_iter;
		}

		// clang-format off
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend auto
		operator<=>( map_iterator const &lhs, map_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return lhs.m_iter <=> rhs.m_iter;
		}
		// clang-format on
	};
	template<Iterator I, typename F>
	map_iterator( I, F ) -> map_iterator<I, F>;

	template<Iterator I, typename F, typename P>
	map_iterator( I, F, P ) -> map_iterator<I, F, P>;

	template<Range R, typename Fn, typename Projection = std::identity>
	struct map_view
	  : range_base_t<map_iterator<iterator_t<R>, Fn, Projection>,
	                 map_iterator<iterator_end_t<R>, Fn, Projection>> {
		using daw_range_base_t =
		  range_base_t<map_iterator<iterator_t<R>, Fn, Projection>,
		               map_iterator<iterator_end_t<R>, Fn, Projection>>;

		using typename daw_range_base_t::iterator_first_t;
		using typename daw_range_base_t::iterator_last_t;

		using value_type = daw::iter_value_t<iterator_first_t>;

		daw::remove_rvalue_ref_t<R> m_range{ };
		iterator_first_t m_first{ };
		iterator_last_t m_last{ };

		explicit map_view( ) = default;

		explicit constexpr map_view( Range auto &&r, Fn const &fn )
		  requires( not Iterator<Fn> )
		  : m_range( DAW_FWD( r ) )
		  , m_first{ std::begin( m_range ), fn }
		  , m_last{ std::end( m_range ), fn } {}

		explicit constexpr map_view( Range auto &&r, Fn const &fn,
		                             Projection const &projection )
		  requires( not Iterator<Fn> and not Iterator<Projection> )
		  : m_range( DAW_FWD( r ) )
		  , m_first{ std::begin( m_range ), fn, projection }
		  , m_last{ std::end( m_range ), fn, projection } {}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator_first_t begin( ) const {
			return m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator_last_t end( ) const {
			return m_last;
		}
	};
	template<Range R, typename F>
	requires( not Iterator<F> ) //
	  map_view( R &&, F ) -> map_view<daw::remove_rvalue_ref_t<R>, F>;

	template<Range R, typename F, typename P>
	requires( not Iterator<F> and not Iterator<P> ) //
	  map_view( R &&, F, P ) -> map_view<daw::remove_rvalue_ref_t<R>, F, P>;

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
					return map_view<R, Fn, Projection>(
					  DAW_FWD( r ), m_func, m_projection );
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
		  DAW_FWD( lo ), DAW_FWD( hi ), DAW_FWD( compare ) };
	}
} // namespace daw::pipelines
