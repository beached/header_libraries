// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_compiler_fixups.h"
#include "daw/traits/daw_traits_concepts.h"

namespace daw::algorithm {
	template<typename InputIterator, typename UnaryPredicate>
	[[nodiscard]] constexpr InputIterator find_first_of( InputIterator first,
	                                                     InputIterator last,
	                                                     UnaryPredicate pred ) {

		(void)daw::traits::is_input_iterator_test<InputIterator>( );
		(void)daw::traits::is_unary_predicate_test<UnaryPredicate,
		                                           decltype( *first )>( );

		while( first != last ) {
			if( pred( *first ) ) {
				break;
			}
			++first;
		}
		return first;
	}

	template<typename InputIterator, typename ForwardIterator>
	[[nodiscard]] constexpr InputIterator find_first_of( InputIterator first,
	                                                     InputIterator last,
	                                                     ForwardIterator sfirst,
	                                                     ForwardIterator slast ) {

		(void)daw::traits::is_input_iterator_test<InputIterator>( );
		(void)daw::traits::is_forward_access_iterator_test<ForwardIterator>( );

		while( first != last ) {
			for( auto it = sfirst; it != slast; ++it ) {
				if( *first == *it ) {
					return first;
				}
			}
			++first;
		}
		return last;
	}

	template<typename InputIterator,
	         typename InputIteratorLast,
	         typename UnaryPredicate>
	constexpr InputIterator find_if( InputIterator first,
	                                 InputIteratorLast last,
	                                 UnaryPredicate &&unary_predicate ) {
		daw::traits::is_input_iterator_test<InputIterator>( );
		daw::traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		DAW_UNSAFE_BUFFER_FUNC_START
		while( first != last ) {
			if( unary_predicate( *first ) ) {
				return first;
			}
			++first;
		}
		DAW_UNSAFE_BUFFER_FUNC_STOP
		return last;
	}
} // namespace daw::algorithm
