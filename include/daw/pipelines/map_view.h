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
#include "daw/daw_typeof.h"
#include "daw/iterator/daw_arrow_proxy.h"
#include "pipeline_traits.h"

#include <cstddef>
#include <functional>
#include <iterator>
#include <tuple>
#include <type_traits>

namespace daw::pipelines {
	template<Iterator Iterator, typename Function>
	struct map_iterator {
		using iterator_category = typename Iterator::iterator_category;
		using value_type = daw::remove_cvref_t<
		  std::invoke_result_t<Function, daw::iter_value_t<Iterator>>>;
		using reference = value_type;
		using const_reference = value_type;
		using pointer = arrow_proxy<value_type>;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

	private:
		Iterator m_iter;
		Function m_func;

	public:
		explicit constexpr map_iterator( ) = default;
		explicit constexpr map_iterator( Iterator it, Function f )
		  : m_iter( it )
		  , m_func( f ) {}

	private:
		[[nodiscard]] constexpr decltype( auto ) raw_get( size_type n )
		  requires( RandomIterator<Iterator> ) {
			return *( m_iter + static_cast<difference_type>( n ) );
		}

		[[nodiscard]] constexpr decltype( auto ) raw_get( size_type n ) const
		  requires( RandomIterator<Iterator> ) {
			return *( m_iter + static_cast<difference_type>( n ) );
		}

	public:
		[[nodiscard]] constexpr value_type operator[]( size_type n ) const
		  requires( RandomIterator<Iterator> ) {
			return std::invoke( m_func, raw_get( n ) );
		}

		[[nodiscard]] constexpr value_type operator*( ) {
			return std::invoke( m_func, *m_iter );
		}

		[[nodiscard]] constexpr value_type operator*( ) const {
			return std::invoke( m_func, *m_iter );
		}

		[[nodiscard]] constexpr pointer operator->( ) {
			return poiner( std::invoke( m_func, *m_iter ) );
		}

		[[nodiscard]] constexpr pointer operator->( ) const {
			return poiner( std::invoke( m_func, *m_iter ) );
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
		  requires( BidirectionalIterator<Iterator> ) {
			--m_iter;
			return *this;
		}

		[[nodiscard]] constexpr map_iterator operator--( int )
		  requires( BidirectionalIterator<Iterator> ) {
			map_iterator result = *this;
			--m_iter;
			return result;
		}

		constexpr map_iterator &operator+=( difference_type n )
		  requires( RandomIterator<Iterator> ) {
			m_iter += n;
			return *this;
		}

		constexpr map_iterator &operator-=( difference_type n )
		  requires( RandomIterator<Iterator> ) {
			m_iter -= n;
			return *this;
		}

		[[nodiscard]] constexpr map_iterator
		operator+( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			map_iterator result = *this;
			m_iter += n;
			return result;
		}

		[[nodiscard]] constexpr map_iterator
		operator-( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			map_iterator result = *this;
			m_iter -= n;
			return result;
		}

		[[nodiscard]] constexpr difference_type operator-( map_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return m_iter - rhs.m_iter;
		}

		[[nodiscard]] constexpr friend bool operator==( map_iterator const &lhs,
		                                                map_iterator const &rhs ) {
			return lhs.m_iter == rhs.m_iter;
		}

		[[nodiscard]] constexpr friend bool operator!=( map_iterator const &lhs,
		                                                map_iterator const &rhs ) {
			return lhs.m_iter != rhs.m_iter;
		}

		[[nodiscard]] constexpr friend bool operator<( map_iterator const &lhs,
		                                               map_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return lhs.m_iter < rhs.m_iter;
		}

		[[nodiscard]] constexpr friend bool operator<=( map_iterator const &lhs,
		                                                map_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return lhs.m_iter <= rhs.m_iter;
		}

		[[nodiscard]] constexpr friend bool operator>( map_iterator const &lhs,
		                                               map_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return lhs.m_iter > rhs.m_iter;
		}

		[[nodiscard]] constexpr friend bool operator>=( map_iterator const &lhs,
		                                                map_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return lhs.m_iter >= rhs.m_iter;
		}
	};
	template<Iterator I, typename F>
	map_iterator( I, F ) -> map_iterator<I, F>;

	template<Iterator Iterator, typename Fn>
	struct map_view {
		using value_type = daw::iter_value_t<map_iterator<Iterator, Fn>>;
		using iterator = map_iterator<Iterator, Fn>;

		iterator m_first = iterator{ };
		iterator m_last = iterator{ };

		explicit map_view( ) = default;

		explicit constexpr map_view( Iterator first, Fn const &fn )
		  : m_first( first, fn ) {}

		explicit constexpr map_view( Iterator first, Iterator last, Fn const &fn )
		  : m_first( first, fn )
		  , m_last( last, fn ) {}

		[[nodiscard]] constexpr iterator begin( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return m_last;
		}
	};
	template<typename I, typename F>
	map_view( I, F ) -> map_view<I, F>;
	template<typename I, typename F>
	map_view( I, I, F ) -> map_view<I, F>;

	template<typename Fn>
	[[nodiscard]] constexpr auto Map( Fn const &fn ) {
		return [func = DAW_FWD( fn )]( auto &&r ) {
			using R = DAW_TYPEOF( r );
			if constexpr( Range<R> ) {
				static_assert( std::is_invocable_v<Fn, range_reference_t<R>>,
				               "Map requires the function to be able to be called "
				               "with invoke and the range_reference_t" );
				static_assert( traits::NoVoidResults<Fn, range_reference_t<R>>,
				               "Map requires the result to not be void" );
				return map_view( std::begin( r ), std::end( r ), func );
			} else {
				static_assert( std::is_invocable_v<Fn, R>,
				               "Map requires the function to be able to be called "
				               "with invoke and passed value" );
				static_assert( traits::NoVoidResults<Fn, R>,
				               "Map requires the result to not be void" );
				return std::invoke( func, DAW_FWD( r ) );
			}
		};
	};

	template<typename Fn>
	[[nodiscard]] constexpr auto MapApply( Fn const &fn ) {
		return [func = DAW_FWD( fn )]<Range R>( R &&r ) {
			static_assert( traits::is_applicable_v<Fn, range_reference_t<R>>,
			               "MapApply requires the function to be able to be called "
			               "with apply and the range_reference_t" );
			static_assert( traits::NoVoidApplyResults<Fn, range_reference_t<R>>,
			               "MapApply requires the result to not be void" );
			return map_view( std::begin( r ), std::end( r ), [=]( auto &&tp ) {
				return std::apply( func, tp );
			} );
		};
	}

	template<typename T, typename Compare = std::less<>>
	[[nodiscard]] constexpr auto
	Clamp( T const &lo, T const &hi, Compare compare = Compare{ } ) {
		return [=]( auto &&r ) {
			using R = DAW_TYPEOF( r );
			if constexpr( Range<R> ) {
				using value_type = range_value_t<R>;
				static_assert(
				  std::convertible_to<T, value_type>,
				  "Clamp requires a lo/hi values convertible to the range value type" );
				return map_view(
				  std::begin( r ), std::end( r ), [=]( value_type const &v ) {
					  return std::clamp( v, lo, hi, compare );
				  } );
			} else {
				static_assert(
				  std::convertible_to<T, daw::remove_cvref_t<R>>,
				  "Clamp requires a lo/hi values convertible to the value type" );
				return std::clamp( r, lo, hi, compare );
			}
		};
	}
} // namespace daw::pipelines
