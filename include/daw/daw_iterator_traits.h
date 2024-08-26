// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_typeof.h"

#include <iterator>
#include <type_traits>

namespace daw {
	template<typename It>
	using iter_value_t =
	  typename std::iterator_traits<std::remove_cvref_t<It>>::value_type;

	template<typename It>
	using iter_reference_t =
	  typename std::iterator_traits<daw::remove_cvref_t<It>>::reference;

	template<typename It>
	using iter_pointer_t =
	  typename std::iterator_traits<daw::remove_cvref_t<It>>::pointer;

	template<typename It>
	using iter_const_reference_t =
	  std::common_reference_t<iter_value_t<It> const &&, iter_reference_t<It>>;

	template<typename It>
	using iter_category_t =
	  typename std::iterator_traits<daw::remove_cvref_t<It>>::iterator_category;

	template<typename It>
	concept Iterator = requires( It & it ) {
		{ ++it } -> std::same_as<It &>;
		{ it++ };
		{ *it };
	};

	template<typename ItCat, typename IteratorCategory>
	concept IsIteratorTag = std::is_base_of_v<IteratorCategory, ItCat>;

	template<typename Tag>
	concept OutputIteratorTag = std::is_base_of_v<std::output_iterator_tag, Tag>;

	template<typename Tag>
	concept InputIteratorTag = std::is_base_of_v<std::input_iterator_tag, Tag>;

	template<typename It>
	concept InputIterator =
	  Iterator<It> and
	  IsIteratorTag<iter_category_t<It>, std::input_iterator_tag>;

	template<typename Tag>
	concept ForwardIteratorTag =
	  std::is_base_of_v<std::forward_iterator_tag, Tag>;

	template<typename It>
	concept ForwardIterator =
	  InputIterator<It> and
	  IsIteratorTag<iter_category_t<It>, std::forward_iterator_tag>;

	template<typename Tag>
	concept BidirectionalIteratorTag =
	  std::is_base_of_v<std::bidirectional_iterator_tag, Tag>;

	template<typename It>
	concept BidirectionalIterator =
	  ForwardIterator<It> and
	  IsIteratorTag<iter_category_t<It>, std::bidirectional_iterator_tag>;

	template<typename Tag>
	concept RandomIteratorTag =
	  std::is_base_of_v<std::random_access_iterator_tag, Tag>;

	template<typename It>
	concept RandomIterator =
	  BidirectionalIterator<It> and
	  IsIteratorTag<iter_category_t<It>, std::random_access_iterator_tag>;

	template<typename IteratorTag, typename... Its>
	inline constexpr bool are_all_same_iterator_tag_v =
	  ( IsIteratorTag<Its, IteratorTag> and ... );

	template<typename... ItCats>
	using common_iterator_category_t = std::conditional_t<
	  are_all_same_iterator_tag_v<std::random_access_iterator_tag, ItCats...>,
	  std::random_access_iterator_tag,
	  std::conditional_t<
	    are_all_same_iterator_tag_v<std::bidirectional_iterator_tag, ItCats...>,
	    std::bidirectional_iterator_tag,
	    std::conditional_t<
	      are_all_same_iterator_tag_v<std::forward_iterator_tag, ItCats...>,
	      std::forward_iterator_tag,
	      std::conditional_t<
	        are_all_same_iterator_tag_v<std::input_iterator_tag, ItCats...>,
	        std::input_iterator_tag,
	        std::conditional_t<
	          are_all_same_iterator_tag_v<std::output_iterator_tag, ItCats...>,
	          std::output_iterator_tag,
	          void>>>>>;

	template<typename R>
	concept Range = requires( R const &r ) {
		{ std::begin( r ) };
		{ std::end( r ) };
	};

	template<Range R>
	using iterator_t = DAW_TYPEOF( std::begin( std::declval<R>( ) ) );

	template<Range R>
	using const_iterator_t =
	  DAW_TYPEOF( std::cbegin( std::declval<R const &>( ) ) );

	template<Range R>
	using range_value_t = iter_value_t<iterator_t<R>>;

	template<Range R>
	using range_reference_t = iter_reference_t<iterator_t<R>>;

	template<Range R>
	using range_const_reference_t = iter_const_reference_t<iterator_t<R>>;

	template<Range R>
	using range_category_t = iter_category_t<iterator_t<R>>;

	[[nodiscard]] constexpr bool empty_range( Range auto &&r ) {
		return std::begin( r ) == std::end( r );
	}

	template<typename R>
	concept InputRange = Range<R> and InputIterator<iterator_t<R>>;

	template<typename R>
	concept ForwardRange = Range<R> and ForwardIterator<iterator_t<R>>;

	template<typename R>
	concept BidirectionalRange =
	  Range<R> and BidirectionalIterator<iterator_t<R>>;

	template<typename R>
	concept RandomRange = Range<R> and RandomIterator<iterator_t<R>>;
} // namespace daw
