// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_exception.h"
#include "daw_not_null.h"
#include "daw_parser_helper.h"

#include <limits>

DAW_UNSAFE_BUFFER_FUNC_START

namespace daw::parser {
	template<typename ForwardIterator>
	constexpr auto find_number( ForwardIterator first, ForwardIterator last ) {
		auto const is_first = []( auto const &v ) {
			return is_a( '-', v ) or is_number( v );
		};

		bool has_decimal = false;
		auto const is_last = [&has_decimal]( auto const &v ) {
			if(is_a( '.', v )) {
				if(has_decimal) {
					return true;
				}
				has_decimal = true;
			}
			return is_number( v );
		};
		auto result = from_to_pred( first, last, is_first, is_last );
		++result.last;
		return result;
	}

	template<typename R>
	constexpr R from_num_char( char c ) {
		auto r = static_cast<unsigned>(static_cast<unsigned char>(c)) - static_cast<
			         unsigned>(static_cast<unsigned char>('0'));
		return static_cast<R>(r);
	}

	template<typename ForwardIterator, typename Result>
	constexpr void parse_unsigned_int( ForwardIterator first,
	                                   ForwardIterator last, Result &result ) {
		result = Result{};
		auto count = std::numeric_limits<Result>::digits10;

		daw::exception::precondition_check<ParserOutOfRangeException>(
			'-' != *first,
			"Negative values are unsupported" );

		for(; first != last and count > 0; ++first, --count) {
			result *= Result{10};
			Result const val = from_num_char<Result>( *first );
			result += val;
		}
		daw::exception::precondition_check<ParserOutOfRangeException>(
			first == last,
			"Not enough room to store number" );
	}

	template<typename Result, typename ForwardIterator>
	constexpr Result parse_unsigned_int( ForwardIterator first,
	                                     ForwardIterator last ) {
		Result result = Result{};
		parse_unsigned_int( first, last, result );
		return result;
	}

	template<typename Result>
	constexpr Result parse_unsigned_int( daw::not_null<char const *> cstring ) {
		daw::not_null<char const *> last = cstring;
		while(*last) {
			++last;
		}
		Result result = Result{};
		parse_unsigned_int( cstring, last, result );
		return result;
	}


	template<typename ForwardIterator, typename Result>
	constexpr void parse_int( ForwardIterator first, ForwardIterator last,
	                          Result &result ) {
		bool is_neg = false;
		if('-' == *first) {
			daw::exception::precondition_check<ParserOutOfRangeException>(
				std::numeric_limits<Result>::is_signed,
				"Negative values are unsupported with unsigned Result" );

			is_neg = true;
			++first;
		}
		result = parse_unsigned_int<Result>( first, last );
		if(is_neg) {
			result *= static_cast<Result>(-1);
		}
	}

	template<typename Result, typename ForwardIterator>
	constexpr Result parse_int( ForwardIterator first, ForwardIterator last ) {
		Result result = Result{};
		parse_int( first, last, result );
		return result;
	}

	template<typename ForwardIterator>
	constexpr auto parse_string_literal( ForwardIterator first,
	                                     ForwardIterator const last ) {
		auto result = trim_left( first, last );
		auto quote_char = *first;

		daw::exception::precondition_check<ParserException>(
			is_quote( quote_char ) );

		auto it = result.first;
		auto last_it = it;
		++it;
		bool found = false;
		while(it != last) {
			if(( found = is_a( *it, quote_char ) ) and !is_escape( *last_it )) {
				break;
			}
			last_it = it;
			++it;
		}
		daw::exception::precondition_check<ParserException>( found );

		++result.first; // trim quote
		result.last = it;
		result.found = true;
		return result;
	}

} // namespace daw::parser
DAW_UNSAFE_BUFFER_FUNC_STOP