// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_swap.h"
#include "daw_traits.h"

// These algorithms require cswap
namespace daw::algorithm {

	/// @brief Performs a left rotation on a range of elements.
	/// @tparam ForwardIterator type of Iterator for items in range
	/// @tparam LastType type that is equal to ForwardIterator when end of range
	/// reached
	/// @param first first item in range
	/// @param middle middle of range, first item in new range
	/// @param last last item in range
	template<typename ForwardIterator, typename LastType>
	constexpr void rotate(
	  ForwardIterator first,
	  ForwardIterator middle,
	  LastType last ) noexcept( noexcept( daw::cswap( *first, *middle ) ) ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_inout_iterator_test<ForwardIterator>( );

		ForwardIterator tmp = middle;
		while( first != tmp ) {
			daw::cswap( *first, *tmp );
			++first;
			++tmp;
			if( tmp == last ) {
				tmp = middle;
			} else if( first == middle ) {
				middle = tmp;
			}
		}
	}

	template<typename ForwardIterator>
	constexpr std::pair<ForwardIterator, ForwardIterator>
	slide( ForwardIterator first, ForwardIterator last, ForwardIterator target ) {

		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );

		if( target < first ) {
			return std::make_pair( target,
			                       daw::algorithm::rotate( target, first, last ) );
		}

		if( last < target ) {
			return std::make_pair( daw::algorithm::rotate( first, last, target ),
			                       target );
		}

		return std::make_pair( first, last );
	}

	template<typename RandomIterator, typename Compare = std::less<>>
	constexpr void nth_element(
	  RandomIterator first,
	  RandomIterator nth,
	  RandomIterator const last,
	  Compare comp =
	    Compare{ } ) noexcept( noexcept( comp( *first, *nth ) )
	                             && noexcept( daw::cswap( *first, *nth ) ) ) {

		traits::is_random_access_iterator_test<RandomIterator>( );
		traits::is_inout_iterator_test<RandomIterator>( );

		traits::is_compare_test<Compare, decltype( *first ), decltype( *nth )>( );

		if( not( first != last ) ) {
			return;
		}
		while( first != nth ) {
			auto min_idx = first;
			auto j = daw::next( first );
			while( j != last ) {
				if( daw::invoke( comp, *j, *min_idx ) ) {
					min_idx = j;
					daw::cswap( *first, *min_idx );
				}
				++j;
			}
			++first;
		}
	}

	template<typename ForwardIterator, typename UnaryPredicate>
	constexpr ForwardIterator partition( ForwardIterator first,
	                                     ForwardIterator last,
	                                     UnaryPredicate &&unary_predicate ) {
		// Precondition checks
		traits::is_forward_access_iterator_test<ForwardIterator>( );
		traits::is_input_iterator_test<ForwardIterator>( );
		traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

		first = find_if_not( first, last, unary_predicate );
		if( first == last ) {
			return first;
		}

		for( auto it = std::next( first ); it != last; ++it ) {
			if( unary_predicate( *it ) ) {
				daw::iter_swap( it, first );
				++first;
			}
		}
		return first;
	}
} // namespace daw::algorithm
