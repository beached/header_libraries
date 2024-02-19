// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

namespace daw::algorithm {
	template<typename ForwardIterator, typename BinaryPredicate>
	constexpr ForwardIterator adjacent_find( ForwardIterator first,
	                                         ForwardIterator last,
	                                         BinaryPredicate p ) {
		if( first == last ) {
			return last;
		}
		ForwardIterator next = first;
		++next;
		while( next != last ) {
			if( p( *first, *next ) ) {
				return first;
			}
			++next;
			++first;
		}
		return last;
	}

} // namespace daw::algorithm
