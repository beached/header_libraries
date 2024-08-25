// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_files
//

#pragma once

#include "daw/daw_traits.h"
#include "daw/impl/daw_make_trait.h"

#include <deque>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace daw::traits {
#define GENERATE_IS_STD_CONTAINER1( ContainerName )                \
	template<typename T>                                             \
	inline constexpr bool is_##ContainerName##_v =                   \
	  std::is_same_v<T, std::ContainerName<typename T::value_type>>; \
	template<typename T>                                             \
	using is_##ContainerName = std::bool_constant<is_##ContainerName##_v<T>>

	GENERATE_IS_STD_CONTAINER1( vector );
	GENERATE_IS_STD_CONTAINER1( list );
	GENERATE_IS_STD_CONTAINER1( set );
	GENERATE_IS_STD_CONTAINER1( unordered_set );
	GENERATE_IS_STD_CONTAINER1( deque );

#undef GENERATE_IS_STD_CONTAINER1

#define GENERATE_IS_STD_CONTAINER2( ContainerName )                     \
	template<typename T>                                                  \
	inline constexpr bool is_##ContainerName##_v = std::is_same_v<        \
	  T,                                                                  \
	  std::ContainerName<typename T::key_type, typename T::mapped_type>>; \
	template<typename T>                                                  \
	using is_##ContainerName = std::bool_constant<is_##ContainerName##_v<T>>

	GENERATE_IS_STD_CONTAINER2( map );
	GENERATE_IS_STD_CONTAINER2( unordered_map );

#undef GENERATE_IS_STD_CONTAINER2

	template<typename T>
	using is_single_item_container = std::disjunction<is_vector<T>,
	                                                  is_list<T>,
	                                                  is_set<T>,
	                                                  is_deque<T>,
	                                                  is_unordered_set<T>>;

	template<typename T>
	inline constexpr bool is_single_item_container_v =
	  is_single_item_container<T>::value;

	template<typename T>
	using is_container = std::disjunction<is_vector<T>,
	                                      is_list<T>,
	                                      is_set<T>,
	                                      is_deque<T>,
	                                      is_unordered_set<T>,
	                                      is_map<T>,
	                                      is_unordered_map<T>>;

	template<typename T>
	inline constexpr bool is_container_v = is_container<T>::value;

	template<typename T>
	using is_map_type = std::disjunction<is_map<T>, is_unordered_map<T>>;

	template<typename T>
	inline constexpr bool is_map_type_v = is_map_type<T>::value;

	template<typename T>
	using is_container_or_array =
	  std::disjunction<is_container<T>, std::is_array<T>>;

	template<typename T>
	inline constexpr bool is_container_or_array_v =
	  is_container_or_array<T>::value;

	template<typename T>
	inline constexpr bool is_string_v =
	  all_true_v<std::is_convertible_v<T, std::string> or
	             std::is_convertible_v<T, std::wstring>>;

	template<typename T>
	inline constexpr bool isnt_string_v = not is_string_v<T>;

	template<typename T>
	inline constexpr bool is_container_not_string_v =
	  all_true_v<isnt_string_v<T>, is_container_like_v<T>>;

	template<typename T>
	inline constexpr bool is_vector_like_not_string_v =
	  all_true_v<is_container_not_string_v<T>, isnt_map_like_v<T>>;

	DAW_MAKE_REQ_TRAIT( has_to_string_v,
	                    std::string( std::declval<T>( ).to_string( ) ) );
} // namespace daw::traits
