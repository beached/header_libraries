// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include <array>
#include <ciso646>
#include <cstddef>
#include <limits>

namespace daw {
	inline constexpr std::size_t DynamicExtent =
	  std::numeric_limits<std::size_t>::max( );

	template<typename Container>
	struct container_traits {
		using container_type = Container;
		using value_type = typename container_type::value_type;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		using size_type = typename container_type::size_type;
		using difference_type = typename container_type::difference_type;
		static constexpr std::size_t extent = DynamicExtent;
		static constexpr bool has_deep_copy = true;
	};

	template<typename T, std::size_t N>
	struct container_traits<T[N]> {
		using container_type = T[N];
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using iterator = T *;
		using const_iterator = T const *;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		static constexpr std::size_t extent = N;
		static constexpr bool has_deep_copy = false;
	};

	template<typename T, std::size_t N>
	struct container_traits<std::array<T, N>> {
		using container_type = std::array<T, N>;
		using value_type = typename container_type::value_type;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		using size_type = typename container_type::size_type;
		using difference_type = typename container_type::difference_type;
		static constexpr std::size_t extent = N;
		static constexpr bool has_deep_copy = true;
	};
} // namespace daw

#if defined( __has_include ) and __has_include( <version> )
#include <version>
#endif

#if defined( __cpp_lib_span )
#include <span>

namespace daw {
	template<typename T, std::size_t Extent>
	struct container_traits<std::span<T, Extent>> {
		using container_type = std::span<T, Extent>;
		using value_type = typename container_type::value_type;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using iterator = typename container_type::iterator;
		using size_type = typename container_type::size_type;
		using difference_type = typename container_type::difference_type;
		static constexpr std::size_t extent = Extent;
		static constexpr bool has_deep_copy = false;
	};
} // namespace daw

#endif
