// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_bit_cast.h"
#include "daw_deduced_type.h"

namespace daw {
	template<typename T, typename Iterator, typename Function>
	void
	bit_cast_for_each( Iterator first, Iterator last, Function &&func ) noexcept(
	  noexcept( func( std::declval<T>( ) ) ) ) {

		while( first != last ) {
			daw::invoke( func, DAW_BIT_CAST( T, *first ) );
			++first;
		}
	}

	template<typename T,
	         typename U = deduced_type,
	         typename Iterator,
	         typename OutputIterator,
	         typename Function>
	void bit_cast_transform( Iterator first,
	                         Iterator last,
	                         OutputIterator first_out,
	                         Function &&func ) {

		using out_t =
		  conditional_t<std::is_same_v<U, deduced_type>,
		                typename std::iterator_traits<OutputIterator>::value_type,
		                U>;

		while( first != last ) {
			*first_out =
			  DAW_BIT_CAST( out_t, daw::invoke( func, DAW_BIT_CAST( T, *first ) ) );
			++first;
			++first_out;
		}
	}

	template<typename T = deduced_type,
	         typename Iterator,
	         typename OutputIterator>
	void
	bit_cast_copy( Iterator first, Iterator last, OutputIterator first_out ) {
		using out_t =
		  conditional_t<std::is_same_v<T, deduced_type>,
		                typename std::iterator_traits<OutputIterator>::value_type,
		                T>;

		while( first != last ) {
			*first_out = DAW_BIT_CAST( out_t, *first );
			++first;
			++first_out;
		}
	}
} // namespace daw
