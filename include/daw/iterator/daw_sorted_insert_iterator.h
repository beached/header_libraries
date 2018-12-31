// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <deque>
#include <functional>
#include <list>
#include <vector>

#include "daw_function_iterator.h"

namespace daw {
	template<typename... Args, typename Compare = std::less<>>
	decltype( auto ) make_sorted_insert_iterator( std::vector<Args...> &c,
	                                              Compare compare = Compare{} ) {
		return daw::make_function_iterator(
		  [&c, compare = std::move( compare )]( auto &&value ) {
			  c.emplace(
			    std::lower_bound( std::begin( c ), std::end( c ), value, compare ),
			    std::forward<decltype( value )>( value ) );
		  } );
	}

	template<typename... Args, typename Compare = std::less<>>
	decltype( auto ) make_sorted_insert_iterator( std::deque<Args...> &c,
	                                              Compare compare = Compare{} ) {
		return daw::make_function_iterator(
		  [&c, compare = std::move( compare )]( auto &&value ) {
			  c.emplace(
			    std::lower_bound( std::begin( c ), std::end( c ), value, compare ),
			    std::forward<decltype( value )>( value ) );
		  } );
	}

	template<typename... Args, typename Compare = std::less<>>
	decltype( auto ) make_sorted_insert_iterator( std::list<Args...> &c,
	                                              Compare compare = Compare{} ) {
		return daw::make_function_iterator(
		  [&c, compare = std::move( compare )]( auto &&value ) {
			  auto const pos =
			    std::lower_bound( std::begin( c ), std::end( c ), value, compare );
			  c.emplace( pos, std::forward<decltype( value )>( value ) );
		  } );
	}

} // namespace daw
