// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_algorithm.h"
#include "daw/daw_attributes.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"

#include <complex>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace daw::pipelines::pimpl {
	template<std::size_t C, typename Needle, typename Projection,
	         typename... ProjectedValues>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto
	finder( Needle const &needle, Projection const &projection,
	        ForwardIterator auto it_current,
	        ProjectedValues &&...projected_values )
	  requires( not InputIterator<Needle> and not InputIterator<Projection> ) //
	{

		static_assert( C >= 0 );
		if constexpr( C == 0 ) {
			static_assert( std::invocable<Needle, ProjectedValues...> );
			return std::invoke( needle, DAW_FWD( projected_values )... );
		} else {
			auto next = it_current;
			static_assert( std::invocable<Projection, decltype( *next )>,
			               "Could not project iterators value_type" );
			++it_current;
			return finder<C - 1>( needle, projection, std::move( it_current ),
			                      DAW_FWD( projected_values )...,
			                      std::invoke( projection, *std::move( next ) ) );
		}
	}

	template<typename Needle, typename Projection = std::identity>
	struct FindValue_t {
		Needle needle;
		DAW_NO_UNIQUE_ADDRESS Projection projection;

		template<ForwardRange R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			auto first = std::begin( r );
			auto const last = std::end( r );
			while( first != last ) {
				if( needle == std::invoke( projection, *first ) ) {
					return first;
				}
				++first;
			}
			return first;
		}
	};

	template<std::size_t FindWidth, typename Needle,
	         typename Projection = std::identity>
	requires( FindWidth > 0 ) //
	  struct FindIf_t {
		DAW_NO_UNIQUE_ADDRESS Needle needle;
		DAW_NO_UNIQUE_ADDRESS Projection projection;

		template<ForwardRange R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			auto first = std::begin( r );
			auto const last = std::end( r );
			while( first != last ) {
				ForwardIterator auto p0 = first;
				Iterator auto plast = daw::safe_next( first, last, FindWidth );
				if( static_cast<std::size_t>( std::distance( p0, plast ) ) <
				    FindWidth ) {
					return last;
				}
				if( finder<FindWidth>( needle, projection, p0 ) ) {
					return first;
				}
				++first;
			}
			return first;
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	/// Find and Adjacent Find depending on FindWidth
	/// When FindWidth == 1, Find - Needle can be a range_value_type or callable
	/// When FindWidth > 1, Adjacent Find - Needle must be a callable taking
	/// FindWidth parameters of type range_value_type
	template<std::size_t FindWidth = 1, typename Needle,
	         typename Projection = std::identity>
	requires( not Range<Needle> ) //
	  [[nodiscard]] constexpr auto FindIf(
	    Needle &&needle, Projection &&projection = Projection{ } ) {
		return pimpl::FindIf_t<FindWidth, Needle, Projection>{
		  DAW_FWD( needle ), DAW_FWD( projection ) };
	}

	template<typename Needle, typename Projection = std::identity>
	requires( not Range<Needle> ) //
	  [[nodiscard]] constexpr auto Find(
	    Needle &&needle, Projection &&projection = Projection{ } ) {
		return pimpl::FindValue_t<Needle, Projection>{ DAW_FWD( needle ),
		                                               DAW_FWD( projection ) };
	}

	template<std::size_t FindWidth = 1, Range R, typename Needle,
	         typename Projection = std::identity>
	requires( std::invocable<Projection, daw::range_reference_t<R>> ) //
	  [[nodiscard]] constexpr auto FindIf(
	    R &&r, Needle &&needle, Projection &&projection = Projection{ } ) {

		return pimpl::FindIf_t<FindWidth, Needle, Projection>{
		  DAW_FWD( needle ), DAW_FWD( projection ) }( DAW_FWD( r ) );
	}

	template<Range R, typename Needle, typename Projection = std::identity>
	requires( std::invocable<Projection, daw::range_reference_t<R>> ) //
	  [[nodiscard]] constexpr auto Find(
	    R &&r, Needle &&needle, Projection &&projection = Projection{ } ) {

		return pimpl::FindValue_t<Needle, Projection>{
		  DAW_FWD( needle ), DAW_FWD( projection ) }( DAW_FWD( r ) );
	}
} // namespace daw::pipelines
