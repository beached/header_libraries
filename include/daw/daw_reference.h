// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_algorithm.h"
#include "daw_traits.h"
#include "traits/daw_traits_is_ostream_like.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <vector>

namespace daw {
	template<typename T>
	struct Reference {
		using ptr_type = daw::traits::root_type_t<T>;
		ptr_type const *ptr;

		Reference( T const &value )
		  : ptr( &value ) {}

		T const &operator*( ) const {
			return *ptr;
		}

		T const &get( ) const {
			return *ptr;
		}
	}; // struct Reference

	template<typename T>
	bool operator==( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs == *rhs;
	}

	template<typename T>
	bool operator!=( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs != *rhs;
	}

	template<typename T>
	bool operator<( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs < *rhs;
	}

	template<typename T>
	bool operator<=( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs <= *rhs;
	}

	template<typename T>
	bool operator>( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs > *rhs;
	}

	template<typename T>
	bool operator>=( Reference<T> const &lhs, Reference<T> const &rhs ) {
		return *lhs >= *rhs;
	}

	template<typename T>
	auto ref( T const *value ) {
		return Reference<T>( *value );
	}

	template<typename T, typename = void>
	auto ref( T const &value ) {
		return Reference<T>( value );
	}

	template<typename Iterator>
	auto to_reference_vector( Iterator first, Iterator last ) {
		using value_type = daw::traits::root_type_t<decltype( *first )>;
		using values_type = std::vector<daw::Reference<value_type>>;

		values_type result{ };
		while( first != last ) {
			result.push_back( daw::ref<value_type>( *first ) );
		}
		return result;
	}

	template<typename OStream, typename T,
	         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
	                          std::nullptr_t> = nullptr>
	OStream &operator<<( OStream &os, Reference<T> const &ref ) {
		os << *ref;
		return os;
	}
} // namespace daw
