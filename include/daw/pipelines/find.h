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

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace daw::pipelines::pimpl {
	template<std::size_t C>
	[[nodiscard]] DAW_ATTRIB_INLINE constexpr auto finder( auto const &needle,
	                                                       auto &&it_current,
	                                                       auto const &last,
	                                                       auto &&...its ) {
		static_assert( C >= 0 );
		if constexpr( C == 0 ) {
			return std::invoke( needle, *DAW_FWD( its )... );
		} else {
			auto next = it_current;
			++it_current;
			return finder<C - 1>( needle,
			                      DAW_FWD( it_current ),
			                      last,
			                      DAW_FWD( its )...,
			                      std::move( next ) );
		}
	}

	template<std::size_t FindWidth, typename Needle, typename>
	requires( FindWidth > 0 ) //
	  struct Find_t {
		DAW_NO_UNIQUE_ADDRESS Needle needle;

		template<ForwardRange R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			// Adjacent Find
			auto first = std::begin( r );
			auto const last = std::end( r );
			while( first != last ) {
				auto p0 = first;
				auto plast = daw::safe_next( first, last, FindWidth );
				auto const sz = std::distance( p0, plast );
				if( static_cast<std::size_t>( sz ) < FindWidth ) {
					return last;
				}
				if( finder<FindWidth>( needle, p0, plast ) ) {
					return first;
				}
				++first;
			}
			return first;
		}
	};

	template<typename Needle, typename Projection>
	struct Find_t<1, Needle, Projection> {
		DAW_NO_UNIQUE_ADDRESS Needle needle;
		DAW_NO_UNIQUE_ADDRESS Projection projection;

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			// Normal find
			auto first = std::begin( r );
			auto const last = std::end( r );
			while( first != last ) {
				if constexpr( requires {
					              std::invoke( needle,
					                           std::invoke( projection, *first ) );
				              } ) {
					if( std::invoke( needle, std::invoke( projection, *first ) ) ) {
						break;
					}
				} else {
					if( std::equal_to<>{ }( needle,
					                        std::invoke( projection, *first ) ) ) {
						break;
					}
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
	template<typename Needle, typename Projection = std::identity>
	requires( not Range<Needle> ) //
	  [[nodiscard]] constexpr auto Find(
	    Needle &&needle, Projection &&projection = Projection{ } ) {
		return pimpl::Find_t<1, Needle, Projection>( DAW_FWD( needle ),
		                                             DAW_FWD( projection ) );
	}

	template<typename Needle, typename Projection = std::identity>
	[[nodiscard]] constexpr auto Find( Range auto &&r,
	                                   Needle &&needle,
	                                   Projection &&projection = Projection{ } ) {
		return Find( DAW_FWD( needle ), DAW_FWD( projection ) )( DAW_FWD( r ) );
	}

	template<std::size_t FindWidth, typename Needle>
	requires( FindWidth > 1 and not Range<Needle> )
	  [[nodiscard]] constexpr auto Find( Needle &&needle ) {
		return pimpl::Find_t<FindWidth, Needle, std::identity>( DAW_FWD( needle ) );
	}

	template<std::size_t FindWidth, typename Needle>
	requires( FindWidth > 1 )
	  [[nodiscard]] constexpr auto Find( Range auto &&r, Needle &&needle ) {
		return Find<FindWidth>( DAW_FWD( needle ) )( DAW_FWD( r ) );
	}
} // namespace daw::pipelines
