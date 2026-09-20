// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/cpp_17.h"
#include "daw/daw_constant.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_remove_cvref.h"
#include <daw/daw_ref_storage.h>

#include <cstddef>
#include <daw/stdinc/tuple_traits.h>

namespace daw::pipelines::pimpl {

	template<typename Result = std::ptrdiff_t>
	[[nodiscard]] constexpr Result ranges_distance( Iterator auto first,
	                                                Iterator auto const &last ) {
		if constexpr( requires { last - first; } ) {
			return static_cast<Result>( last - first );
		} else {
			Result d = Result{ };
			while( first != last ) {
				++first;
				++d;
			}
			return d;
		}
	}

	template<typename Result = std::ptrdiff_t>
	[[nodiscard]] constexpr Result ranges_distance( Range auto const &r ) {
		return ranges_distance<Result>( std::begin( r ), std::end( r ) );
	}

	template<typename First, typename Last = First>
	struct range_base_t {
		using i_am_a_daw_pipelines_range_base_t = void;
		using iterator_first_t = First;
		using iterator_last_t = Last;

		range_base_t( ) = default;
	};

	template<typename R>
	concept RangeBase = requires {
		typename daw::remove_cvref_t<R>::i_am_a_daw_pipelines_range_base_t;
	};

	template<Range R, typename First = iterator_t<R>,
	         typename Last = iterator_end_t<R>,
	         typename CFirst = const_iterator_t<R>,
	         typename CLast = const_iterator_end_t<R>>
	struct stored_range_base_t : private pimpl::range_base_t<First, Last> {
		using daw_range_base_t = range_base_t<First, Last>;
		using iterator_first_t = typename daw_range_base_t::iterator_first_t;
		using iterator_last_t = typename daw_range_base_t::iterator_last_t;
		using const_iterator_first_t = CFirst;
		using const_iterator_last_t = CLast;
		using m_range_t = daw::ref_storage<R>;

	private:
		m_range_t m_range;

	public:
		explicit stored_range_base_t( ) = default;

		constexpr stored_range_base_t( R r )
		  : m_range( DAW_FWD( r ) ) {}

		[[nodiscard]] constexpr auto &get( ) {
			return m_range.get( );
		}

		[[nodiscard]] constexpr auto const &get( ) const {
			return m_range.get( );
		}

		[[nodiscard]] constexpr decltype( auto ) rbegin( ) {
			return std::begin( get( ) );
		}

		[[nodiscard]] constexpr decltype( auto ) rbegin( ) const {
			return std::begin( get( ) );
		}

		[[nodiscard]] constexpr decltype( auto ) rend( ) {
			return std::end( get( ) );
		}

		[[nodiscard]] constexpr decltype( auto ) rend( ) const {
			return std::end( get( ) );
		}

		[[nodiscard]] constexpr bool
		operator==( stored_range_base_t const &rhs ) const
		  requires( std::equality_comparable<daw::remove_cvref_t<R>> ) {
			return get( ) == rhs.get( );
		}

		[[nodiscard]] constexpr bool
		operator!=( stored_range_base_t const &rhs ) const {
			return get( ) != rhs.get( );
		}

		[[nodiscard]] constexpr decltype( auto ) operator*( ) {
			return *get( );
		}

		[[nodiscard]] constexpr decltype( auto ) operator*( ) const {
			return *get( );
		}

		[[nodiscard]] constexpr decltype( auto ) operator++( ) {
			return ++get( );
		}
	};

	template<Range... Rs>
	struct variant_range_storage_t {
		using range_type = std::variant<stored_range_base_t<Rs>...>;
		template<std::size_t Idx>
		using elem_type = std::variant_alternative_t<Idx, range_type>;
		range_type m_range;

		variant_range_storage_t( ) = default;

		template<std::size_t Index, Range R>
		constexpr variant_range_storage_t( R &&r, constant<Index> )
		  : m_range{ range_type{ std::in_place_index<Index>, DAW_FWD( r ) } } {}
		template<std::size_t Index>
		constexpr decltype( auto ) get( ) & {
			return std::get<Index>( m_range );
		}

		template<std::size_t Index>
		constexpr decltype( auto ) get( ) const & {
			return std::get<Index>( m_range );
		}

		template<std::size_t Index>
		constexpr decltype( auto ) get( ) && {
			return std::get<Index>( std::move( m_range ) );
		}

		template<std::size_t Index>
		constexpr decltype( auto ) get( ) const && {
			return std::get<Index>( std::move( m_range ) );
		}

		constexpr decltype( auto ) visit( auto &&visitor ) & {
			return daw::visit( m_range, [&]( auto &&v ) -> decltype( auto ) {
				return DAW_FWD( visitor )( DAW_FWD( v ) );
			} );
		}

		constexpr decltype( auto ) visit( auto &&visitor ) const & {
			return daw::visit( m_range, [&]( auto &&v ) -> decltype( auto ) {
				return DAW_FWD( visitor )( DAW_FWD( v ) );
			} );
		}

		constexpr decltype( auto ) visit( auto &&visitor ) && {
			return daw::visit( m_range, [&]( auto &&v ) -> decltype( auto ) {
				return DAW_FWD( visitor )( DAW_FWD( v ) );
			} );
		}

		constexpr decltype( auto ) visit( auto &&visitor ) const && {
			return daw::visit( m_range, [&]( auto &&v ) -> decltype( auto ) {
				return DAW_FWD( visitor )( DAW_FWD( v ) );
			} );
		}

		[[nodiscard]] constexpr bool
		operator==( variant_range_storage_t const &rhs ) const {
			if( m_range.index( ) != rhs.m_range.index( ) ) {
				return false;
			}
			return [&]<std::size_t... Is>( std::index_sequence<Is...> ) -> bool {
				return (
				  ( Is == m_range.index( ) and *std::get_if<Is>( &m_range ) ==
				                                 *std::get_if<Is>( &rhs.m_range ) ) or
				  ... );
			}( std::make_index_sequence<sizeof...( Rs )>{ } );
		}

		[[nodiscard]] constexpr bool
		operator!=( variant_range_storage_t const &rhs ) const {
			return not( *this == rhs );
		}
	};
} // namespace daw::pipelines::pimpl

namespace std {
	template<daw::pipelines::pimpl::RangeBase R>
	inline constexpr std::size_t tuple_size_v<R> = 2;

	template<daw::pipelines::pimpl::RangeBase R>
	struct tuple_size<R> {
		static constexpr std::size_t value = 2;
	};

	template<std::size_t I, daw::pipelines::pimpl::RangeBase R>
	struct tuple_element<I, R> {
		using type = typename R::iterator_first_t;
	};

	template<daw::pipelines::pimpl::RangeBase R>
	struct tuple_element<1, R> {
		using type = typename R::iterator_last_t;
	};

	template<std::size_t I>
	constexpr auto get( daw::pipelines::pimpl::RangeBase auto &&r )
	  requires( I == 0 or I == 1 ) {
		if constexpr( I == 0 ) {
			return DAW_FWD( r ).begin( );
		} else {
			return DAW_FWD( r ).end( );
		}
	}
} // namespace std
