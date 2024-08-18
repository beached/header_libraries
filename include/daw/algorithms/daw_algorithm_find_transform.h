// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/stdinc/iterator_traits.h>
#include <optional>

namespace daw::algorithm {
	template<typename Iterator>
	struct find_transform_result_t {
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		Iterator iterator;
		std::optional<value_type> value = std::nullopt;

		explicit constexpr operator bool( ) const noexcept {
			return value.has_value( );
		}
	};

	/// A C++ implementation of Rusts find_map.
	/// It minimizes the cost when the transformation is expensive and
	/// only calls it once per element until the predicate returns true
	/// on the transformed result
	template<typename Iterator,
	         typename IteratorLast,
	         typename Func,
	         typename Pred>
	constexpr find_transform_result_t<Iterator> find_transform( Iterator first,
	                                                            IteratorLast last,
	                                                            Func &&func,
	                                                            Pred &&pred ) {
		while( first != last ) {
			auto result = func( *first );
			if( pred( result ) ) {
				return { first, std::move( result ) };
			}
			++first;
		}
		return { first };
	}
} // namespace daw::algorithm
