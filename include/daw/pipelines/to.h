// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_move.h"
#include "range.h"

#include <iterator>
#include <type_traits>

namespace daw::pipelines {
	template<template<typename...> typename Container>
	[[nodiscard]] constexpr auto To( ) {
		// inline constexpr auto To = impl::To_t<Container>{ };
		return []<typename R>( R &&r ) {
			if constexpr( Range<R> ) {
				return Container( std::begin( DAW_FWD( r ) ),
				                  std::end( DAW_FWD( r ) ) );
			} else {
				auto mv = maybe_view{ r };
				return Container{ std::begin( mv ), std::end( mv ) };
			}
		};
	}

	namespace impl {

		template<typename T, std::size_t N>
		void array_test( std::array<T, N> const & );

		struct UseTypeDefault {};
	} // namespace impl

	template<typename Container>
	requires( not requires( Container c ) { impl::array_test( c ); } )
	  [[nodiscard]] constexpr auto To( ) {
		// inline constexpr auto To = impl::To_t<Container>{ };
		return []<typename R>( R &&r ) {
			if constexpr( Range<R> ) {
				return Container( std::begin( DAW_FWD( r ) ),
				                  std::end( DAW_FWD( r ) ) );
			} else {
				auto mv = maybe_view{ r };
				return Container{ std::begin( mv ), std::end( mv ) };
			}
		};
	}

	template<typename Array, auto Default = impl::UseTypeDefault{ }>
	requires( requires( Array a ) { impl::array_test( a ); } )
	  [[nodiscard]] constexpr auto To( ) {
		// inline constexpr auto To = impl::To_t<Container>{ };
		return []<typename R>( R &&r ) {
			constexpr auto sz = std::tuple_size_v<Array>;
			using value_t = typename Array::value_type;
			static_assert(
			  std::convertible_to<range_reference_t<R>, value_t>,
			  "Destination type is not compatible with the Range value types" );
			auto first = std::begin( r );
			auto const last = std::end( r );
			auto const make_value = [&]( std::size_t ) {
				if( first != last ) {
					return *first++;
				}
				if constexpr( std::is_same_v<decltype( Default ),
				                             impl::UseTypeDefault> ) {
					return value_t{ };
				} else {
					return Default;
				}
			};
			return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
				return std::array<value_t, sz>{ make_value( Is )... };
			}( std::make_index_sequence<sz>{ } );
		};
	}
} // namespace daw::pipelines
