// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_parser_helper.h"

namespace daw {
	namespace parser {
		template<typename ForwardIterator>
		constexpr auto find_number( ForwardIterator first, ForwardIterator last ) {
			using namespace daw::parser;
			auto is_first = []( auto const &v ) { return is_a( '-', v ) || is_number( v ); };

			bool has_decimal = false;
			auto is_last = [&has_decimal]( auto const &v ) {
				if( is_a( '.', v ) ) {
					if( has_decimal ) {
						return true;
					}
					has_decimal = true;
				}
				return is_number( v );
			};
			return from_to( first, last, is_first, is_last );
		}

		template<typename ForwardIterator, typename Result>
		constexpr void parse_unsigned_int( ForwardIterator first, ForwardIterator last, Result &result ) {
			size_t count = std::numeric_limits<Result>::digits10;

			if( '-' == *first ) {
				throw ParserOutOfRangeException{"Negative values are unsupported"};
			}
			result = 0;
			for( ; first != last && count > 0; ++first, --count ) {
				result *= static_cast<Result>( 10 );
				Result val = *first - '0';
				result += val;
			}
			if( first != last ) {
				throw ParserOutOfRangeException{"Not enough room to store unsigned integer"};
			}
		}

		template<typename ForwardIterator, typename Result>
		constexpr void parse_int( ForwardIterator first, ForwardIterator last, Result &result ) {
			size_t count = std::numeric_limits<Result>::digits10;
			result = 0;
			bool is_neg = false;
			if( '-' == *first ) {
				if( !std::numeric_limits<Result>::is_signed ) {
					throw ParserOutOfRangeException{"Negative values are unsupported with unsigned Result"};
				}
				is_neg = true;
				++first;
			}
			for( ; first != last && count > 0; ++first, --count ) {
				result *= static_cast<Result>( 10 );
				Result val = *first - '0';
				result += val;
			}
			if( first != last ) {
				throw ParserOutOfRangeException{"Not enough room to store signed integer"};
			}
			if( is_neg ) {
				result *= static_cast<Result>( -1 );
			}
		}

		template<typename ForwardIterator>
		auto parse_string_literal( ForwardIterator first, ForwardIterator const last ) {
			auto result = left_trim( first, last );
			auto quote_char = *first;
			if( !is_quote( quote_char ) ) {
				throw ParserException{};
			}
			auto it = result.first;
			auto last_it = it;
			++it;
			bool found = false;
			while( it != last ) {
				if( ( found = is_a( *it, quote_char ) ) && !is_escape( *last_it ) ) {
					break;
				}
				last_it = it++;
			}
			if( !found ) {
				throw ParserException{};
			}
			++result.first; // trim quote
			result.last = it;
			result.found = true;
			return result;
		}

	} // namespace parser
} // namespace daw
