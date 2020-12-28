// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_traits.h"

#include <ciso646>

namespace daw {
	namespace range {
		namespace impl {
			template<typename Collection>
			auto to_vector( Collection const &collection ) {
				using value_type =
				  daw::traits::root_type_t<typename Collection::value_type>;
				std::vector<value_type> result;
				std::copy( std::begin( collection ), std::end( collection ),
				           std::back_inserter( result ) );
				return result;
			}

			template<typename IteratorF, typename IteratorL>
			auto to_vector( IteratorF first, IteratorL last ) {
				using value_type = typename std::iterator_traits<IteratorF>::value_type;
				std::vector<value_type> result;
				std::copy( first, last, std::back_inserter( result ) );
				return result;
			}

			template<typename T>
			using cleanup_t = daw::traits::root_type_t<T>;

		} // namespace impl

		template<typename Arg, typename... Args>
		auto make_range_reference( Arg &&arg, Args &&...args );
		template<typename Arg, typename... Args>
		auto make_range_collection( Arg &&arg, Args &&...args );
	} // namespace range
} // namespace daw
