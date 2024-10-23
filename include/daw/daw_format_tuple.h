// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_string_view.h"
#include "daw_iterator_traits.h"
#include "impl/formatter_common.h"

#include <cassert>
#include <format>
#include <string_view>

namespace daw {
	template<typename T, typename CharT = formatter_impl::DefaultCharT>
	struct fmt_tuple {
		T const &value;
		daw::basic_string_view<CharT> Separator =
		  formatter_impl::DefaultSeparator<CharT>;
		daw::basic_string_view<CharT> Left = formatter_impl::DefaultLeft<CharT>;
		daw::basic_string_view<CharT> Right = formatter_impl::DefaultRight<CharT>;

		explicit constexpr fmt_tuple( T const &c )
		  : value( c ) {}

		explicit constexpr fmt_tuple( T const &c, CharT const *separator )
		  : value( c )
		  , Separator( separator ) {}

		explicit constexpr fmt_tuple( T const &c, CharT const *separator,
		                              CharT const *left, CharT const *right )
		  : value( c )
		  , Separator( separator )
		  , Left( left )
		  , Right( right ) {}
	};
	template<typename T>
	fmt_tuple( T ) -> fmt_tuple<T>;

	template<typename T, typename CharT>
	fmt_tuple( T, CharT const * ) -> fmt_tuple<T, CharT>;

	template<typename T, typename CharT>
	fmt_tuple( T, CharT const *, CharT const *, CharT const * )
	  -> fmt_tuple<T, CharT>;
} // namespace daw

namespace std {
	template<typename T, typename CharT>
	struct formatter<daw::fmt_tuple<T, CharT>, CharT> {
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
		Ctx::iterator format( daw::fmt_tuple<T, CharT> c, Ctx &ctx ) const {
			auto out = ctx.out( );
			out = std::copy( std::data( c.Left ), daw::data_end( c.Left ), out );
			bool is_first = true;
			auto const write_value =
			  [&]<std::size_t Idx>( std::integral_constant<std::size_t, Idx> ) {
				  if( is_first ) {
					  is_first = false;
				  } else {
					  out = std::copy( std::data( c.Separator ),
					                   daw::data_end( c.Separator ), out );
				  }
				  out = std::vformat_to( out, std::string_view( flags ),
				                         std::make_format_args( get<Idx>( c.value ) ) );
			  };
			[&]<std::size_t... Is>( std::index_sequence<Is...> ) {
				(void)( write_value( std::integral_constant<std::size_t, Is>{ } ), ...,
				        1 );
			}( std::make_index_sequence<std::tuple_size_v<T>>{ } );
			out = std::copy( std::data( c.Right ), daw::data_end( c.Right ), out );
			return out;
		}
	};
} // namespace std
