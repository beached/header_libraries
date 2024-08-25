// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace daw {
	template<typename, typename = void>
	struct extract_from_container_t;

	template<typename, typename = void>
	struct insert_into_container_t;

	template<typename T>
	constexpr auto extract_from_container( T &container ) {
		return extract_from_container_t<T>{ }( container );
	}

	template<typename Container, typename Allocator, typename Pointer>
	constexpr void insert_into_container( Container &vec,
	                                      Pointer *buff,
	                                      Allocator alloc,
	                                      std::size_t capacity,
	                                      std::size_t size ) {
		insert_into_container_t<Container>{ }(
		  vec, buff, std::move( alloc ), capacity, size );
	}
} // namespace daw
