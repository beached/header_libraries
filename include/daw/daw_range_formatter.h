// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_iterator_traits.h"

#include <format>

namespace daw {
	template<Range R>
	struct fmt_range {
		R const &container;

		explicit constexpr fmt_range( R const &c )
		  : container( c ) {}
	};
	template<Range R>
	fmt_range( R ) -> fmt_range<R>;
} // namespace daw

namespace std {
	template<daw::Range R>
	struct formatter<daw::fmt_range<R>, char> {
		template<class ParseContext>
		constexpr ParseContext::iterator parse( ParseContext &ctx ) {
			return ctx.begin( );
		}

		template<class FmtContext>
		FmtContext::iterator format( daw::fmt_range<R> c, FmtContext &ctx ) const {
			using value_t = daw::range_value_t<R>;
			if constexpr( std::is_same_v<char, value_t> ) {
				if constexpr( requires { std::data( c.container ); } ) {
					return std::ranges::copy( c.container, ctx.out( ) ).out;
				} else {
					auto s = std::string{ };
					// Print as string
					if constexpr( daw::ForwardRange<R> ) {
						s.resize( static_cast<std::size_t>( std::distance(
						  std::begin( c.container ), std::end( c.container ) ) ) );
					}
					(void)std::copy( std::begin( c.container ),
					                 std::end( c.container ),
					                 std::begin( s ) );
					return std::ranges::copy( std::move( s ), ctx.out( ) ).out;
				}
			} else {
				auto s = std::string{ };
				s += "[";
				bool is_first = true;
				for( auto const &v : c.container ) {
					if( is_first ) {
						is_first = false;
						s += std::format( "{}", v );
					} else {
						s += std::format( ", {}", v );
					}
				}
				s += ']';
				return std::ranges::copy( std::move( s ), ctx.out( ) ).out;
			}
		}
	};
} // namespace std
