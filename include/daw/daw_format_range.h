// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_string_view.h"
#include "daw_format_tuple.h"
#include "daw_iterator_traits.h"
#include "impl/formatter_common.h"

#include <cassert>
#include <format>
#include <string_view>

namespace daw {
	template<Range R, typename CharT = formatter_impl::DefaultCharT>
	struct fmt_range {
		R const &container;
		daw::basic_string_view<CharT> Separator =
		  formatter_impl::DefaultSeparator<CharT>;
		daw::basic_string_view<CharT> Left = formatter_impl::DefaultLeft<CharT>;
		daw::basic_string_view<CharT> Right = formatter_impl::DefaultRight<CharT>;

		explicit constexpr fmt_range( R const &c )
		  : container( c ) {}

		explicit constexpr fmt_range( R const &c, CharT const *separator )
		  : container( c )
		  , Separator( separator ) {}

		explicit constexpr fmt_range( R const &c,
		                              CharT const *separator,
		                              CharT const *left,
		                              CharT const *right )
		  : container( c )
		  , Separator( separator )
		  , Left( left )
		  , Right( right ) {}
	};
	template<Range R>
	fmt_range( R ) -> fmt_range<R>;

	template<Range R, typename CharT>
	fmt_range( R, CharT const * ) -> fmt_range<R, CharT>;

	template<Range R, typename CharT>
	fmt_range( R, CharT const *, CharT const *, CharT const * )
	  -> fmt_range<R, CharT>;
} // namespace daw

namespace std {
	template<daw::Range R, typename CharT>
	struct formatter<daw::fmt_range<R, CharT>, CharT> {
		daw::string_view flags = { };

		formatter( ) = default;

		template<class ParseContext>
		constexpr ParseContext::iterator parse( ParseContext &ctx ) {
			auto f = daw::string_view( ctx.begin( ), ctx.end( ) );
			if( ctx.begin( ) == ctx.end( ) or *ctx.begin( ) == '}' ) {
				flags = "{}";
				return ctx.begin( );
			}
			int brace_count = 1;
			bool in_quote = false;
			auto const *first = f.data( );
			auto const *last = first;
			while( not f.empty( ) ) {
				switch( f.front( ) ) {
				case '"':
					in_quote = not in_quote;
					break;
				case '{':
					if( not in_quote ) {
						++brace_count;
					}
					break;
				case '}':
					if( not in_quote ) {
						--brace_count;
						assert( brace_count >= 0 );
						if( brace_count == 0 ) {
							last = f.data( ) + 1;
							f.remove_prefix( f.size( ) - 1 );
						}
					}
					break;
				default:
					break;
				}
				f.remove_prefix( );
			}
			flags =
			  daw::string_view( first, last ); // static_cast<std::size_t>( sz ) );
			flags.expand_prefix( 2 );
			assert( flags.front( ) == '{' );
			auto const sz = last - first;
			return ctx.begin( ) + sz - 1;
			//			return ctx.end( ) - 1;
		}

		template<typename Ctx>
		Ctx::iterator format( daw::fmt_range<R, CharT> c, Ctx &ctx ) const {
			using value_t = daw::range_value_t<R>;
			if constexpr( std::is_same_v<daw::formatter_impl::DefaultCharT,
			                             value_t> ) {
				// format as string
				auto out = std::copy(
				  std::begin( c.container ), std::end( c.container ), ctx.out( ) );
				return out;
			} else {
				auto out = ctx.out( );
				out = std::copy( std::data( c.Left ), daw::data_end( c.Left ), out );
				bool is_first = true;
				for( auto const &v : c.container ) {
					if( is_first ) {
						is_first = false;
					} else {
						out = std::copy(
						  std::data( c.Separator ), daw::data_end( c.Separator ), out );
					}
					if constexpr( daw::is_tuple_like_v<daw::range_value_t<R>> ) {
						auto t = daw::fmt_tuple( v );
						out = std::vformat_to(
						  out, std::string_view( flags ), std::make_format_args( t ) );
					} else {
						out = std::vformat_to(
						  out, std::string_view( flags ), std::make_format_args( v ) );
					}
				}
				out = std::copy( std::data( c.Right ), daw::data_end( c.Right ), out );
				return out;
			}
		}
	};
} // namespace std
